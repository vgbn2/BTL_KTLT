(() => {
  function setupReport() {
    const pathname = window.location.pathname;
    const isReport = pathname.includes('/Reports/') || document.querySelector('.architecture-report-page') !== null;
    document.body.classList.toggle('architecture-report-page', isReport);
    if (window.mermaid) {
      const dark = document.body.getAttribute('data-md-color-scheme') === 'slate';
      window.mermaid.initialize({
        startOnLoad: true,
        securityLevel: 'strict',
        theme: dark ? 'dark' : 'base',
        flowchart: { htmlLabels: false, curve: 'basis', useMaxWidth: true },
      });
      if (typeof window.mermaid.run === 'function') {
        window.mermaid.run({ querySelector: '.mermaid' }).catch(() => {});
      }
    }
  }

  if (window.document$ && typeof window.document$.subscribe === 'function') {
    window.document$.subscribe(setupReport);
  }
  document.addEventListener('DOMContentLoaded', setupReport, { once: true });
})();
