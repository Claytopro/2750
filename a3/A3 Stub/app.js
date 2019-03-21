/*
Clayton Provan
march 16 2018
node server utilies
*/
'use strict'

// C library API
const ffi = require('ffi');
const ref = require("ref");//for the c pointer

var ICALobj = ref.types.void;
var ICALptr = ref.refType(ICALobj);

let parser = ffi.Library("./libcal.so",{
  // main writer gedcom
  "nodeCreateCal": [ICALptr,["string"]],
  "nodeWriteCAl": ["void", ["string", ICALptr]],

  //JSON Fucntions
  "calendarToJSON":["string",[ICALptr]],
  "nodeEventListJSON":["string",[ICALptr]],
  "nodeAlarmListJSON":["string",["string","int"]],
  "nodePropertieListJSON":["string",["string","int"]],
  "addEventToCalendar":["void",["string","string","string","string","string"]],
  "nodeCreateNewCalendar":["void",["string","string","string","string","string","string"]]

});

function tester(){
  let testFile = "./uploads/test3.ics";
  console.log('pretest');
  let calObj = parser.nodeCreateCal(testFile);
  let str2 = parser.nodeEventListJSON(calObj);
  let str = parser.calendarToJSON(calObj);
  console.log('str:' + str);
  console.log('str2:'+ str2);
  parser.nodeWriteCAl("nodejsmadethis.ics",calObj);
}



// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {

  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    console.log(err);
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ********************

//Sample endpoint
app.get('/uploads', function(req , res){
  const fileNames = [];
  const testFolder = './uploads/';
  const fs = require('fs');
  fs.readdirSync(testFolder).forEach(file => {
    fileNames.push(file);

  });
  console.log('sending files');
  res.send(fileNames);
});

app.get('/uploadObjs',function(req,res){
  const fileNames = [];
  const testFolder = './uploads/';
  const fs = require('fs');
  fs.readdirSync(testFolder).forEach(file => {
    fileNames.push(file);
  });

  var calJSONs = [];

  for(var i=0; i<fileNames.length;i++){
    var file = "./uploads/" + fileNames[i];
    console.log("creating for " + file);

    var cal = parser.nodeCreateCal(file);
    var jsonCal = parser.calendarToJSON(cal);

    var calOBJ = [];
    calOBJ = JSON.parse(jsonCal);
    calJSONs.push(calOBJ);
  }


  res.send(calJSONs);
});

app.get('/getEvts', function(req , res){
    var filename = req.query.fileSelected;
    var file = "./uploads/" + filename;
    console.log(" file path = " + file);
    var cal = parser.nodeCreateCal(file);
    var jsonString = parser.nodeEventListJSON(cal);
    console.log("parsed events = " + jsonString.toString());


    res.send(jsonString);
});

app.get('/getAlarms', function(req , res){
    var filename = req.query.fileSelected;
    var file = "./uploads/" + filename;
    var select = parseInt(req.query.evtSelected);
    console.log("pre json parse: " + file);

    var jsonString = parser.nodeAlarmListJSON(file,select);
    console.log("parsed alarms = " + jsonString.toString());

    res.send(jsonString);
});

app.get('/getProperties', function(req , res){
    var filename = req.query.fileSelected;
    var file = "./uploads/" + filename;
    var select = parseInt(req.query.evtSelected);
    console.log("pre json parse: " + file);

    var jsonString = parser.nodePropertieListJSON(file,select);
    console.log("parsed propperties = " + jsonString.toString());

    res.send(jsonString);
});


app.get('/addEvent', function(req , res){
    var filename = req.query.fileSelected;
    var file = "./uploads/" + filename;
    var evt = req.query.event;
    var cDate = req.query.creationDate;
    var sDate = req.query.startDate;
    var sumProp= req.query.sumProp;
    parser.addEventToCalendar(file,sDate,cDate,evt,sumProp);

    console.log("SuccessFully added event");
    res.send("Upload SuccessFull");
});

app.get('/createCal', function(req , res){
    var filename = req.query.fileSelected;
    var file = "./uploads/" + filename;
    var calendarJSON = req.query.calendarInfo;
    console.log("started input file:" + file);
    console.log("cal: "+ calendarJSON);


    var evt = req.query.event;
    var cDate = req.query.creationDate;
    var sDate = req.query.startDate;
    var sumProp= req.query.sumProp;

    parser.nodeCreateNewCalendar(file,calendarJSON,sDate,cDate,evt,sumProp);

    console.log("SuccessFully created Calendar");
    res.send("Upload SuccessFull");
});



app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
