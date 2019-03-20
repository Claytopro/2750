


function addText(){
  let div = document.getElementById('statusDiv');

  div.innerHTML +=  'placeholder <br />';
}


function clearText(divID){
  let div = document.getElementById(divID);
  div.innerHTML = '';
  return false;
}

// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    //On page-load AJAX Example
    let files = [];
    let calendarObjects = [];
    let createEventSelect;
    let tableSelect;
    let ddFile;

    //just gets all the files in uploads
    $.ajax({
        type: 'get',            //Request type
        url: '/uploads/',   //The server endpoint we are connecting to
        success: function (data) {
          files = data;
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });

    $.ajax({
      type:'get',
      url:'/uploadObjs',
      dataType:'json',
      success:function(data){
        let table = document.getElementById("fileTable");
        let dropdown = document.getElementById("ddFiles");
        let eventCreationdropdown = document.getElementById("ddFilesforEvents");
        let div = document.getElementById('statusDiv');

        calendarObjects = data;
        if(data.length == 0){
          document.getElementById('fileForm').innerHTML = 'NO FILES';
        }else{
          document.getElementById("fileTable").style.visibility = "visible";
        }

        for(let i =0;i<data.length;i++){

          if(data[i].prodID !== undefined){
            let row = table.insertRow(-1);
            let cellName = row.insertCell(0);
            let cellVersion = row.insertCell(1);
            let cellProdId = row.insertCell(2);
            let cellNumEvts = row.insertCell(3);
            let cellNumProps = row.insertCell(4);

            cellName.innerHTML = '<a href="/uploads/'+ files[i] + '">'+files[i]+'</a>';
            cellVersion.innerHTML = data[i].version;
            cellProdId.innerHTML  = data[i].prodID;
            cellNumEvts.innerHTML  = data[i].numEvents;
            cellNumProps.innerHTML  = data[i].numProps;
            $(dropdown).append( '<li><a class="dropdown-item" href="#" data-value="' + files[i] +'">' + files[i] + '</a></li>' );
            $(eventCreationdropdown).append( '<li><a class="dropdown-item" href="#" data-value="' + files[i] +'">' + files[i] + '</a></li>' );
          }else{
            div.innerHTML += files[i] + ' is invalid and cannot be uploaded <br />';
          }

        }
      },fail:function(error){
        // Non-200 return, do something with error
        console.log(error);
      }

    });

;

    // Event listener form replacement example, building a Single-Page-App, no redirects if possible
    $('#ddFiles').on('click','li a' ,function(){
      $(this).parents(".dropdown").find('.btn').html($(this).text() + ' <span class="caret"></span>');
      $(this).parents(".dropdown").find('.btn').val($(this).data('value'));
      ddFile = $(this).text();
      console.log('test:' + $(this).text());
      $('#calTable tbody').empty();

        $.ajax({
              type: 'get',
              dataType: 'JSON',
              url: '/getEvts',
              data: {fileSelected: ddFile},
              success: function (data) {
                  console.log('rtrn:' + JSON.stringify(data));
                  let info = JSON.parse(JSON.stringify(data));
                  let numEvts = document.getElementById("numEvents");
                  let table = document.getElementById("caltableBody");

                  for(let i=0; i < info.length;i++){
                    let row = table.insertRow(i);
                    let cellNumEvt = row.insertCell(0);
                    let cellstartdt = row.insertCell(1);
                    let cellstartTime = row.insertCell(2);
                    let cellSummary = row.insertCell(3);
                    let cellPros = row.insertCell(4);
                    let cellAlarms = row.insertCell(5);

                    cellNumEvt.innerHTML = i+1;
                    cellstartdt.innerHTML = info[i].startDT.date.slice(0,4) +"/" +info[i].startDT.date.slice(4,6)+"/" +info[i].startDT.date.slice(6,8)  ;
                    if(info[i].startDT.isUTC == true){

                      cellstartTime.innerHTML = info[i].startDT.time.slice(0,2)+":" +info[i].startDT.time.slice(2,4)+":" +info[i].startDT.time.slice(4,6) + "(UTC)";
                    }else{
                        cellstartTime.innerHTML = info[i].startDT.time.slice(0,2)+":" +info[i].startDT.time.slice(2,4)+":" +info[i].startDT.time.slice(4,6);
                    }

                    cellSummary.innerHTML = info[i].summary;
                    cellPros.innerHTML = info[i].numProps;
                    cellAlarms.innerHTML = info[i].numAlarms;
                    numEvts.max = i+1;
                  }

              },
              fail: function(error) {
                  // Non-200 return, do something with error
                  console.log(error);
              }
          });//end ajax

    });


    $('#ddFilesforEvents').on('click','li a' ,function(){
      $(this).parents(".dropdown").find('.btn').html($(this).text() + ' <span class="caret"></span>');
      $(this).parents(".dropdown").find('.btn').val($(this).data('value'));
      createEventSelect = $(this).text();
      console.log('evnt file selected:' + $(this).text());

    });


    $('#crtEvent').click(function(){
      let div = document.getElementById('statusDiv');

      console.log('event clicked');
      //createEventSelect is defined is scope of document rdy

      if(createEventSelect != undefined){
        let evtForm = document.getElementById("evtFrm");
        if(evtFrm[0].value){
          console.log("uid:" + evtFrm[0].value);
          if(evtFrm[1].value){

            if(evtFrm[2].value){

              let evtJSON = "{\"UID\":\""+ evtFrm[0].value + "\"}";
              let date = evtFrm[1].value.toString().slice(0,4);
              date += evtFrm[1].value.toString().slice(5,7) + evtFrm[1].value.toString().slice(8,10);
              let time = evtFrm[2].value.toString().slice(0,2) +evtFrm[2].value.toString().slice(3,5) + "00";
              let dStartJSON = "{\"date\":\""+ date +"\",\"time\":\""+ time +"\",\"isUTC\":false}"

              //gets creation date from current date
              let today = new Date();
              let dd = String(today.getDate()).padStart(2, '0');
              let mm = String(today.getMonth() + 1).padStart(2, '0');
              let yyyy = today.getFullYear();
              let creationTime = today.getHours() + today.getMinutes() + today.getSeconds();
              

              let creationDateTime = "{\"date\":\""+ yyyy +mm+dd +"\",\"time\":\""+ creationTime +"\",\"isUTC\":false}"

              let summary;

              if(evtFrm[3].value){
                summary ="{\"propName\":\"Summary\",\"propDescr\":\""+evtFrm[3].value.toString() +"\"}";
              }else{
                summary = "";
              }

              console.log('Event is ' + evtJSON);
              console.log('creation date =' + creationDateTime);
              console.log('start date = ' + dStartJSON);
              console.log('Summary = ' + summary);
              $.ajax({
              type: 'get',
              url: '/addEvent',
              data: {fileSelected: createEventSelect, event:evtJSON, creationDate:creationDateTime, startDate:dStartJSON,sumProp:summary},
              success: function (data) {
                console.log('added event to file');

              },
              fail: function(error) {
                  // Non-200 return, do something with error
                  console.log(error);
              }
              });//end aja

            }else{
              div.innerHTML += 'No Time Selected, Cannot Create Event <br />';
            }
          }else{
            div.innerHTML += 'No Date Selected, Cannot Create Event <br />';
          }
        }else{
          div.innerHTML += 'No UID input, Cannot Create Event <br />';
        }
      }else{
        div.innerHTML += 'No File Selected, Cannot Create Event <br />';
      }
    });



    $('#viewAlarms').click(function(){
      let div = document.getElementById('statusDiv');
      let numEvts = document.getElementById("numEvents").value;
      console.log(numEvts);
      console.log( document.getElementById("numEvents").max);

      if(numEvts >document.getElementById("numEvents").max || numEvts <document.getElementById("numEvents").min){
        div.innerHTML += 'Selected event does not exist <br />';
      }else{
        console.log("ddfiles =:"+ ddFile);
        $('#displayStubbTbody').empty();

        $.ajax({
              type: 'get',
              dataType: 'JSON',
              url: '/getAlarms',
              data:{fileSelected: ddFile, evtSelected: numEvts},
              success: function (data) {
                let info = JSON.parse(JSON.stringify(data));
                let table = document.getElementById("displayStuffTable");
                let row = table.insertRow(-1);


                let cell = row.insertCell(-1);
                cell.innerHTML = "<h4>Number of Properties</h4>";
                cell = row.insertCell(-1);
                cell.innerHTML = "<h4>Trigger</h4>";
                cell = row.insertCell(-1);
                cell.innerHTML = "<h4>Action</h4>";

                for(let i=0; i < info.length;i++){
                  row = table.insertRow(i+1);

                  cell = row.insertCell(-1);
                  cell.innerHTML = info[i].numProps;
                  cell = row.insertCell(-1);
                  cell.innerHTML = info[i].trigger;
                  cell = row.insertCell(-1);
                  cell.innerHTML = info[i].action;
                }

            //    console.log("alarm stuff + data is " + info[0].trigger);

              },fail: function(error) {
                  // Non-200 return, do something with error
                  console.log(error);
              }

            });
      }

    });

    $('#viewProps').click(function(){
      let div = document.getElementById('statusDiv');
      let numEvts = document.getElementById("numEvents").value;
      console.log(numEvts);
      console.log( document.getElementById("numEvents").max);

      if(numEvts >document.getElementById("numEvents").max || numEvts <document.getElementById("numEvents").min){
        div.innerHTML += 'Selected event does not exist <br />';
      }else{
        console.log("ddfiles =:"+ ddFile);
        $('#displayStubbTbody').empty();

        $.ajax({
              type: 'get',
              dataType: 'JSON',
              url: '/getProperties',
              data:{fileSelected: ddFile, evtSelected: numEvts},
              success: function (data) {
                let info = JSON.parse(JSON.stringify(data));
                let table = document.getElementById("displayStuffTable");
                let row = table.insertRow(-1);


                let cell = row.insertCell(-1);
                cell.innerHTML = "<h4> Property Name </h4>";
                cell = row.insertCell(-1);
                cell.innerHTML = "<h4> Propert Description </h4>";


                for(let i=0; i < info.length;i++){
                  row = table.insertRow(i+1);
                  cell = row.insertCell(-1);
                  cell.innerHTML = info[i].propName;
                  cell = row.insertCell(-1);
                  cell.innerHTML = info[i].propDescr;
                }

            //    console.log("alarm stuff + data is " + info[0].trigger);

              },fail: function(error) {
                  // Non-200 return, do something with error
                  console.log(error);
              }


            });
      }
    });



});
