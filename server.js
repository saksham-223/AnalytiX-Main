const express = require("express");
const fs = require("fs");
const { exec } = require("child_process");

const app = express();

app.use(express.json());
app.use(express.static("ui"));

app.post("/analyze",(req,res)=>{

let code = req.body.code;

fs.writeFileSync("test_code/sample.c",code);

exec("analytiX.exe",(err,stdout,stderr)=>{
if (err) res.send(stdout + "\nError: " + stderr);
else res.send(stdout);
});

});

app.listen(3000,()=>{
console.log("Server running at http://localhost:3000");
});