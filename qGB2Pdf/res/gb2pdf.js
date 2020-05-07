function log(s) {
    //alert(s);
    console.log(s);
    return s;
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
    log("INF: div not found with class '" + classNamePart + "'");
    return false;
}
function getPageContainer(classNamePart) {
    var div = getElementByClassNamePart("div", classNamePart);
    var s = null;
    if (div)
    {
        s = div.firstChild.innerHTML;
        if(!s)
            s = div.innerHTML;
        if (s) {
            return s;
        }
    }
    return log("WRN: content not found class:'" + classNamePart + "' div:'"+div+"' s:'"+s+"' url:" + window.location.href);
}
//alert(getPageContainer("--pageContainer-"));
//alert(clickCardNext("--cardNext-"));
