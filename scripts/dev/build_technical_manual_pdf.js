#!/usr/bin/env node
'use strict';

/**
 * scripts/dev/build_technical_manual_pdf.js
 *
 * Deterministic A4 PDF generator for the KTLT Telecommunications Subscriber
 * Management Platform — Technical Manual & System Architecture Specification.
 * Supports both English and Vietnamese specifications:
 * 1. KTLT_TECHNICAL_MANUAL_AND_SYSTEM_SPECIFICATION.pdf (English)
 * 2. SO_TAY_KY_THUAT_VA_DAC_TA_KIEN_TRUC_HE_THONG_KTLT.pdf (Vietnamese)
 *
 * Pipeline:
 * - Builds MkDocs static site via containerized mkdocs-material
 * - Serves via local ephemeral HTTP server
 * - Renders through headless Chromium with A4 dimensions and no header/footer chrome
 * - Dynamically extracts headings and reconciles TOC page numbers in DOM
 * - Extracts headings and injects hierarchical PDF document bookmarks (/Outlines) via Ghostscript
 * - Validates PDF metadata, page size, and outline dictionaries
 * - Stages all intermediate files in RAM tmpfs (/dev/shm) to eliminate SSD write wear
 * - Atomically commits final PDFs to docs/Reports/
 *
 * ponytail: stdlib first, native puppeteer/chromium, ghostscript pdfmark outline, tmpfs staging.
 */

const fs = require('node:fs');
const http = require('node:http');
const path = require('node:path');
const os = require('node:os');
const { execSync } = require('node:child_process');

const REPO_ROOT = path.resolve(__dirname, '..', '..');
const SITE_DIR = path.join(REPO_ROOT, 'site');

const TARGETS = [
  {
    key: 'en',
    name: 'KTLT Technical Manual & Specification (English)',
    mdPath: path.join(REPO_ROOT, 'docs/Reports/KTLT_TECHNICAL_MANUAL_AND_SYSTEM_SPECIFICATION.md'),
    pdfPath: path.join(REPO_ROOT, 'docs/Reports/KTLT_TECHNICAL_MANUAL_AND_SYSTEM_SPECIFICATION.pdf'),
    urlPath: '/Reports/KTLT_TECHNICAL_MANUAL_AND_SYSTEM_SPECIFICATION/',
  },
  {
    key: 'vn',
    name: 'Sổ Tay Kỹ Thuật & Đặc Tả Hệ Thống (Tiếng Việt)',
    mdPath: path.join(REPO_ROOT, 'docs/Reports/SO_TAY_KY_THUAT_VA_DAC_TA_KIEN_TRUC_HE_THONG_KTLT.md'),
    pdfPath: path.join(REPO_ROOT, 'docs/Reports/SO_TAY_KY_THUAT_VA_DAC_TA_KIEN_TRUC_HE_THONG_KTLT.pdf'),
    urlPath: '/Reports/SO_TAY_KY_THUAT_VA_DAC_TA_KIEN_TRUC_HE_THONG_KTLT/',
  },
];

// Zero-SSD-wear tmpfs directory
const TMP_DIR = fs.existsSync('/dev/shm') ? '/dev/shm' : os.tmpdir();

function resolvePuppeteer() {
  const candidate = '/home/vgbn/.nvm/versions/node/v24.19.0/lib/node_modules/@mermaid-js/mermaid-cli/node_modules/puppeteer';
  if (fs.existsSync(candidate)) {
    return candidate;
  }
  return 'puppeteer';
}

async function serveDirectory(dir) {
  const contentTypes = {
    '.css': 'text/css; charset=utf-8',
    '.html': 'text/html; charset=utf-8',
    '.js': 'text/javascript; charset=utf-8',
    '.json': 'application/json; charset=utf-8',
    '.mjs': 'text/javascript; charset=utf-8',
    '.svg': 'image/svg+xml',
    '.png': 'image/png',
    '.jpg': 'image/jpeg',
    '.woff2': 'font/woff2',
    '.woff': 'font/woff',
    '.ttf': 'font/ttf',
  };

  const server = http.createServer((request, response) => {
    const pathname = decodeURIComponent(new URL(request.url, 'http://127.0.0.1').pathname);
    let target = path.resolve(dir, `.${pathname}`);
    if (!target.startsWith(`${dir}${path.sep}`) && target !== dir) {
      response.writeHead(403).end();
      return;
    }
    if (fs.existsSync(target) && fs.statSync(target).isDirectory()) {
      target = path.join(target, 'index.html');
    }
    if (!fs.existsSync(target)) {
      response.writeHead(404).end('Not Found');
      return;
    }
    const ext = path.extname(target).toLowerCase();
    response.setHeader('content-type', contentTypes[ext] || 'application/octet-stream');
    fs.createReadStream(target).pipe(response);
  });

  await new Promise((resolve) => server.listen(0, '127.0.0.1', resolve));
  const { port } = server.address();
  return { server, port };
}

function extractHeadingsWithPages(markdownPath, rawPdfPath) {
  const mdContent = fs.readFileSync(markdownPath, 'utf8');
  const lines = mdContent.split('\n');
  const headings = [];
  let inCodeFence = false;

  for (const line of lines) {
    if (line.trim().startsWith('```')) {
      inCodeFence = !inCodeFence;
      continue;
    }
    if (inCodeFence) continue;

    const match = line.match(/^(#{1,3})\s+(.+)$/);
    if (!match) continue;
    const level = match[1].length;
    let title = match[2].trim();
    title = title.replace(/\[([^\]]+)\]\([^)]+\)/g, '$1');
    title = title.replace(/[`*_]/g, '');
    if (title.toLowerCase().includes('table of contents') ||
        title.toLowerCase().includes('mục lục') ||
        title.toLowerCase() === 'contents') continue;
    headings.push({ level, title });
  }

  const infoRaw = execSync(`pdfinfo "${rawPdfPath}"`, { encoding: 'utf8' });
  const pageMatch = infoRaw.match(/Pages:\s+(\d+)/);
  const totalPages = pageMatch ? Number.parseInt(pageMatch[1], 10) : 1;

  const pageTexts = [];
  for (let p = 1; p <= totalPages; p++) {
    try {
      const text = execSync(`pdftotext -f ${p} -l ${p} "${rawPdfPath}" -`, { encoding: 'utf8' });
      pageTexts.push(text.toLowerCase());
    } catch (err) {
      pageTexts.push('');
    }
  }

  let lastFoundPage = 1;
  const mapped = [];

  for (const h of headings) {
    const isChapter = !!h.title.match(/^\d+(\.\d+)?/);
    const startSearch = isChapter ? Math.max(lastFoundPage, 3) : lastFoundPage;
    const cleanSearchTarget = h.title
      .replace(/^\d+(\.\d+)*\.?\s*/, '')
      .toLowerCase()
      .trim()
      .slice(0, 30);

    let foundPage = startSearch;

    for (let p = startSearch; p <= totalPages; p++) {
      if (cleanSearchTarget && pageTexts[p - 1].includes(cleanSearchTarget)) {
        foundPage = p;
        lastFoundPage = p;
        break;
      }
    }

    mapped.push({
      level: h.level,
      title: h.title,
      page: foundPage,
    });
  }

  return mapped;
}

function generatePdfmarkOutline(mappedHeadings) {
  let ps = '';
  for (const h of mappedHeadings) {
    const safeTitle = h.title.replace(/\\/g, '\\\\').replace(/\(/g, '\\(').replace(/\)/g, '\\)');
    ps += `[/Title (${safeTitle}) /Page ${h.page} /View [/XYZ null null null] /OUT pdfmark\n`;
  }
  return ps;
}

async function renderTargetPdf(browser, target, port, verbose) {
  if (verbose) console.log(`\n============================================================`);
  if (verbose) console.log(`[PDF BUILD] Processing: ${target.name}`);
  if (verbose) console.log(`============================================================`);

  const page = await browser.newPage();
  const targetUrl = `http://127.0.0.1:${port}${target.urlPath}`;
  if (verbose) console.log(`[PDF BUILD] Navigating to: ${targetUrl}`);

  await page.goto(targetUrl, { waitUntil: 'networkidle0', timeout: 90000 });
  await new Promise((resolve) => setTimeout(resolve, 3500));

  const renderStats = await page.evaluate(() => {
    const diagrams = document.querySelectorAll('pre.stm32-diagram, div.stm32-figure');
    const formulas = document.querySelectorAll('.arithmatex, .MathJax');
    const tables = document.querySelectorAll('table');
    return {
      totalDiagrams: diagrams.length,
      totalFormulas: formulas.length,
      totalTables: tables.length,
    };
  });
  if (verbose) {
    console.log(`[PDF BUILD] Render Stats: ${renderStats.totalDiagrams} schematics, ${renderStats.totalFormulas} formulas, ${renderStats.totalTables} tables.`);
  }

  const rawPdf = path.join(TMP_DIR, `ktlt_${target.key}.raw.${Date.now()}.pdf`);
  if (verbose) console.log(`[PDF BUILD] Printing preliminary PDF to tmpfs for pagination analysis...`);

  await page.pdf({
    path: rawPdf,
    format: 'A4',
    printBackground: true,
    displayHeaderFooter: false,
    margin: {
      top: '14mm',
      right: '13mm',
      bottom: '16mm',
      left: '13mm',
    },
  });

  // Extract headings and inject resolved page numbers into TOC
  if (verbose) console.log('[PDF BUILD] Resolving heading page distribution and updating TOC in DOM...');
  const mappedHeadings = extractHeadingsWithPages(target.mdPath, rawPdf);

  const tocUpdateCount = await page.evaluate((mapped) => {
    let updated = 0;
    const items = document.querySelectorAll('.stm32-toc-item');
    for (const item of items) {
      const textEl = item.querySelector('.stm32-toc-text');
      const numEl = item.querySelector('.stm32-toc-num');
      const pageEl = item.querySelector('.stm32-toc-page');
      if (!pageEl) continue;

      const numText = numEl ? numEl.textContent.trim() : '';
      const cleanNum = numText.replace(/\.$/, '');
      const titleText = textEl ? textEl.textContent.trim().toLowerCase() : '';

      for (const h of mapped) {
        const hTitle = h.title.toLowerCase();
        const isMatch = (numText && (
          hTitle.startsWith(`${cleanNum} `) ||
          hTitle.startsWith(`${cleanNum}. `) ||
          hTitle.startsWith(`${cleanNum} - `) ||
          hTitle.startsWith(`${cleanNum}: `)
        )) || (titleText && hTitle.includes(titleText));

        if (isMatch) {
          pageEl.textContent = String(h.page);
          updated++;
          break;
        }
      }
    }
    return updated;
  }, mappedHeadings);

  if (verbose) console.log(`[PDF BUILD] Updated ${tocUpdateCount} TOC entries with verified page numbers.`);

  // Re-print final PDF with updated TOC
  const finalRawPdf = path.join(TMP_DIR, `ktlt_${target.key}.finalraw.${Date.now()}.pdf`);
  if (verbose) console.log(`[PDF BUILD] Re-printing final A4 PDF with synchronized TOC...`);
  await page.pdf({
    path: finalRawPdf,
    format: 'A4',
    printBackground: true,
    displayHeaderFooter: false,
    margin: {
      top: '14mm',
      right: '13mm',
      bottom: '16mm',
      left: '13mm',
    },
  });

  await page.close();

  // Inject PDF document outline bookmarks via Ghostscript
  if (verbose) console.log('[PDF BUILD] Injecting PDF document outline bookmarks (/Outlines) via Ghostscript...');
  const finalHeadings = extractHeadingsWithPages(target.mdPath, finalRawPdf);
  const pdfmarkSource = generatePdfmarkOutline(finalHeadings);

  const psPath = path.join(TMP_DIR, `ktlt_${target.key}.outlines.${Date.now()}.ps`);
  fs.writeFileSync(psPath, pdfmarkSource, 'utf8');

  const outlinedPdf = path.join(TMP_DIR, `ktlt_${target.key}.final.${Date.now()}.pdf`);
  execSync(`gs -sDEVICE=pdfwrite -dCompatibilityLevel=1.5 -dNOPAUSE -dQUIET -dBATCH -sOutputFile="${outlinedPdf}" "${finalRawPdf}" "${psPath}"`, {
    stdio: verbose ? 'inherit' : 'ignore',
  });

  // Cleanup temporary files in tmpfs
  try { fs.unlinkSync(rawPdf); } catch (err) { void err; }
  try { fs.unlinkSync(finalRawPdf); } catch (err) { void err; }
  try { fs.unlinkSync(psPath); } catch (err) { void err; }

  // Validation
  const infoRaw = execSync(`pdfinfo "${outlinedPdf}"`, { encoding: 'utf8' });
  const pageMatch = infoRaw.match(/Pages:\s+(\d+)/);
  const sizeMatch = infoRaw.match(/Page size:\s+([^\n]+)/);
  const pageCount = pageMatch ? Number.parseInt(pageMatch[1], 10) : 0;
  const pageSize = sizeMatch ? sizeMatch[1].trim() : 'unknown';

  const outlinedRaw = fs.readFileSync(outlinedPdf);
  if (!outlinedRaw.toString('latin1').includes('/Outlines')) {
    throw new Error('PDF missing embedded document outline dictionary (/Outlines)');
  }

  // Atomic Commit to docs/Reports
  fs.mkdirSync(path.dirname(target.pdfPath), { recursive: true });
  fs.copyFileSync(outlinedPdf, target.pdfPath);
  try { fs.unlinkSync(outlinedPdf); } catch (err) { void err; }

  if (verbose) console.log(`[PDF BUILD] Committed canonical PDF to: ${target.pdfPath} (${pageCount} pages, ${pageSize})`);
  return { ok: true, pages: pageCount, pdfPath: target.pdfPath };
}

async function buildAllManualPdfs(options = {}) {
  const verbose = options.verbose ?? true;
  if (verbose) console.log('[PDF BUILD] Step 1: Building MkDocs documentation site via Docker...');

  const dockerCmd = `docker run --rm -v "${REPO_ROOT}:/docs" squidfunk/mkdocs-material:latest build`;
  execSync(dockerCmd, { cwd: REPO_ROOT, stdio: verbose ? 'inherit' : 'ignore' });

  if (verbose) console.log('[PDF BUILD] Step 2: Serving documentation on local loopback server...');
  const { server, port } = await serveDirectory(SITE_DIR);

  try {
    if (verbose) console.log('[PDF BUILD] Step 3: Launching headless Chromium via Puppeteer...');
    const puppeteerModule = resolvePuppeteer();
    const puppeteer = require(puppeteerModule);

    const browser = await puppeteer.launch({
      executablePath: process.env.PUPPETEER_EXECUTABLE_PATH || '/usr/bin/chromium',
      headless: true,
      args: ['--no-sandbox', '--disable-gpu', '--disable-dev-shm-usage'],
    });

    const results = [];
    for (const target of TARGETS) {
      if (fs.existsSync(target.mdPath)) {
        const res = await renderTargetPdf(browser, target, port, verbose);
        results.push({ ...res, name: target.name });
      }
    }

    await browser.close();
    server.close();
    return { ok: true, results };
  } catch (err) {
    server.close();
    throw err;
  }
}

if (require.main === module) {
  buildAllManualPdfs()
    .then((res) => {
      console.log('\n[SUMMARY] Built Technical Manuals:');
      for (const r of res.results) {
        console.log(`  - ${r.name}: ${r.pages} pages -> ${r.pdfPath}`);
      }
      process.exit(0);
    })
    .catch((err) => {
      console.error('[ERROR] Build failed:', err.message);
      process.exit(1);
    });
}

module.exports = { buildAllManualPdfs };
