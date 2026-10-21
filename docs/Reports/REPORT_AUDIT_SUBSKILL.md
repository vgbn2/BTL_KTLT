# Report Audit Subskill

Use this checklist before exporting any report from
`docs/Reports/IMEI_AND_SHARED_LIBRARIES_REPORT.md` to DOCX or PDF.

## Goal

Keep report claims aligned with the current implementation. Treat source code and executable test output as the authority; treat the report as a derived document.

## 1. Establish the audit surface

Read the report and identify claims about:

- class and file names;
- menu features and search fields;
- validation ownership and exception behavior;
- file loading, persistence timing, temporary files, and startup failures;
- compiler, language standard, and Makefile commands;
- test groups and current test counts;
- team modules, stubs, placeholders, and build inclusion;
- external-system boundaries such as EIR, network quality, databases, or APIs.

Then inspect the corresponding source, callers, tests, and build files. Prefer the graph first:

```bash
graphify query "report claims and implementation for <topic>" --budget 1800
```

Use direct search and reads to verify every claim that matters.

## 2. Claim-to-source checks

| Report claim | Required evidence |
|---|---|
| Class/file exists | Header, implementation, and actual build/reference path |
| Menu feature exists | Menu constants, displayed option, handler branch, and exercising test |
| Validation is enforced | Canonical predicate/constructor/setter plus all input paths, including deserialization |
| Exception is reported | Throw site, caller handling, and observable output or test |
| Data is persisted | Mutation path calls `saveToFile`; reload test verifies stored state |
| Load failure is safe | `loadFromFile()` result/exception is handled by startup code; probe missing-file behavior |
| Atomic save is claimed | `.tmp` write, close, rename result, and failure handling |
| C++ standard is claimed | `Makefile` flags plus strict compiler check, preferably `-pedantic-errors` |
| Test count is claimed | Fresh `make test` output; do not copy an old count |
| Module is implemented | Definitions exist, build includes them, and a caller exercises them |
| External integration is claimed | Direct implementation evidence; otherwise label it simulation, placeholder, or future work |

## 3. Required probes

Run the smallest relevant checks:

```bash
make test
make all
g++ -std=c++11 -pedantic-errors -Wall -Wextra <same sources as Makefile> -o /tmp/strict_cxx11

git diff --check
anti-cheat-enforcer --diff --require-tests
```

For persistence claims, use real temporary fixture files. Include at least:

- one valid record that reloads successfully;
- one malformed record that is skipped or rejected as documented;
- one missing/unreadable file with the documented startup result;
- one duplicate-ID and one missing-ID mutation where applicable.

Do not use shallow mocks that bypass parsers, codecs, constructors, or disk state.

## 4. False-positive screen

Dismiss a suspected discrepancy when direct evidence shows it is intentional, for example:

- an offline fixture fallback explicitly documented as such;
- a test-only stub not included in production build;
- a placeholder clearly labeled as planned work;
- a diagnostic warning that does not claim successful persistence;
- a generated artifact whose source file is authoritative.

Every reported discrepancy needs a file/line anchor and a failing boundary. Do not infer behavior from names alone.

## 5. Export gate

Do not regenerate DOCX/PDF until all functional claims are either verified or corrected. After editing the Markdown:

```bash
pandoc docs/Reports/IMEI_AND_SHARED_LIBRARIES_REPORT.md \
  --from=gfm \
  --reference-doc=docs/Reports/IMEI_AND_SHARED_LIBRARIES_REPORT.docx \
  --output=/tmp/IMEI_AND_SHARED_LIBRARIES_REPORT.docx
libreoffice --headless --convert-to pdf --outdir /tmp \
  /tmp/IMEI_AND_SHARED_LIBRARIES_REPORT.docx
```

Validate before replacing repository exports:

```bash
unzip -t /tmp/IMEI_AND_SHARED_LIBRARIES_REPORT.docx
pdfinfo /tmp/IMEI_AND_SHARED_LIBRARIES_REPORT.pdf
pdftotext /tmp/IMEI_AND_SHARED_LIBRARIES_REPORT.pdf - | rg "<current test count>|<corrected claim>"
```

Keep the simple Vietnamese `baocaoktlt` style: direct chapter headings, plain prose, `a)/b)` lists, short library explanations, conclusion, and references.

## Stop conditions

Stop and correct the report or source when:

- a named class or file does not exist;
- a menu option has no handler or test evidence;
- deserialization bypasses domain validation;
- startup ignores a load failure;
- the Makefile standard conflicts with actual syntax;
- a test count is stale;
- a placeholder is presented as production functionality;
- an external integration is claimed without an implementation boundary.

Record the finding in the dated handoff and persistent memory before closing the audit.
