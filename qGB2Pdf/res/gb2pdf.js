function log(s) {
    //alert(s);
    console.log(s);
}
function getElementByClassNamePart(tagName, classNamePart) {
    var divs = document.getElementsByTagName(tagName);
    for(var i=0;i < divs.length; i++) {
        var div = divs[i];
        if (div.className.indexOf(classNamePart) > 1) {
            return div;
        }
    }
    return null;
}
function clickCardNext(classNamePart) {
    var div = getElementByClassNamePart("a", classNamePart);
    if (div) {
        div.click();
        return true;    
    }
    log("div not found with class '" + classNamePart + "'");
    return false;
}
function getPageContainer(classNamePart) {
    var div = getElementByClassNamePart("div", classNamePart);
    if (div) return div.innerHTML;
    log("div not found with class '" + classNamePart + "'");
    return null;
}
//alert(getPageContainer("--pageContainer-"));
//alert(clickCardNext("--cardNext-"));
