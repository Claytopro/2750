


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

        for(var i =0;i<data.length;i++){

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
      var ddFile = $(this).text();
        console.log('test:' + $(this).text());
        $('#calTable tbody').empty();

        $.ajax({
              type: 'get',
              dataType: 'JSON',
              url: '/getEvts',
              data: {fileSelected: ddFile},
              success: function (data) {
                  console.log('rtrn:' + JSON.stringify(data));
                  var info = JSON.parse(JSON.stringify(data));

                  let table = document.getElementById("caltableBody");

                  for(var i=0; i < info.length;i++){
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
      var ddFile = $(this).text();
      console.log('test:' + $(this).text());

    });

    $('#crtEvent').click(function(){
      console.log('event clicked');
      let ddSelect = $(ddFilesforEvents).text()
      console.log('seslct:' + ddSelect);
    });



});
