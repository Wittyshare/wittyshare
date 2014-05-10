
var pageNum = 1;
var pdfjs = null;
var pdfDoc = null;

function load(url){

    pdfjs = PDFJS.getDocument(url);
    pdfjs.then(function getPdfHelloWorld(_pdfDoc) {
        pdfDoc = _pdfDoc;
        pageNum = 1;
        renderPage(pageNum);
    });
}

function renderPage(num) {
    pdfjs.then(function(pdf) {
        pdf.getPage(num).then(function(page) {
            var scale = 1;
            var viewport = page.getViewport(scale);
            var canvas = document.getElementById('the-canvas');
            var context = canvas.getContext('2d');
            canvas.height = viewport.height;
            canvas.width = viewport.width;
            var renderContext = {
                canvasContext: context,
            viewport: viewport
            };
            page.render(renderContext);
        });
    });
    document.getElementById('page_num').textContent = pageNum;
    document.getElementById('page_count').textContent = pdfDoc.numPages;
    if(pageNum == pdfDoc.numPages)
        document.getElementById('next').disabled=true;
    else
        document.getElementById('next').disabled=false;
    if(pageNum == 1)
        document.getElementById('prev').disabled=true;
    else
        document.getElementById('prev').disabled=false;
}

//
// Go to previous page
//
function goPrevious() {
    if (pageNum <= 1)
        return;
    pageNum--;
    renderPage(pageNum);
}

//
// Go to next page
//
function goNext() {
    if (pageNum >= pdfDoc.numPages)
        return;
    pageNum++;
    renderPage(pageNum);
}

//
// Asynchronously download PDF as an ArrayBuffer
//
