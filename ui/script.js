function syncScroll() {
    let textarea = document.getElementById("code");
    let hglt = document.getElementById("highlights");
    let text = textarea.value;
    
    hglt.innerHTML = applyHighlights(text);
    hglt.scrollTop = textarea.scrollTop;
}

let errorLineNum = -1;
let typingTimer;

let editorSize = 18;
let outputSize = 16;

function changeFontSize(amount) {
    editorSize += amount;
    outputSize += amount;
    
    // Bounds check so the text isn't unreadable
    if (editorSize < 10) editorSize = 10;
    if (editorSize > 40) editorSize = 40;
    if (outputSize < 10) outputSize = 10;
    if (outputSize > 40) outputSize = 40;
    
    document.documentElement.style.setProperty('--editor-font-size', editorSize + 'px');
    document.documentElement.style.setProperty('--output-font-size', outputSize + 'px');
    syncScroll(); // recalculate height sync for visual error block lines
}

function toggleTheme() {
    document.body.classList.toggle("dark-mode");
}

function onTextChange() {
    syncScroll();
    clearTimeout(typingTimer);
    // Auto-analyze 500ms after the user stops typing
    typingTimer = setTimeout(analyze, 500); 
}

function applyHighlights(text) {
    if(!text) return "";
    let lines = text.split('\n');
    let out = "";
    for(let i=0; i<lines.length; i++) {
        let lNum = i + 1;
        let escaped = lines[i].replace(/</g, "&lt;").replace(/>/g, "&gt;");
        // Ensure whitespace match
        if(escaped === "") escaped = " ";
        
        if(lNum === errorLineNum) {
            out += `<span class="error-line">${escaped}</span>\n`;
        } else {
            out += escaped + "\n";
        }
    }
    return out;
}

function analyze(){
    let code = document.getElementById("code").value;
    errorLineNum = -1; // reset
    syncScroll();

    fetch("http://localhost:3000/analyze",{
        method:"POST",
        headers:{
            "Content-Type":"application/json"
        },
        body:JSON.stringify({code:code})
    })
    .then(res=>res.text())
    .then(data=>{
        let match = data.match(/\|\|\|ERROR_LINE:(\d+)\|\|\|/);
        if(match) {
            errorLineNum = parseInt(match[1]);
            data = data.replace(match[0], ""); 
            syncScroll(); // Apply the error line styling overlay
        }
        document.getElementById("output").textContent=data;
    });
}