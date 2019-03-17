
function addToFileTable() {
  let table = document.getElementById("fileTable");

   let row = table.insertRow(-1);
   let cellName = row.insertCell(0);
   let cellVer = row.insertCell(1);
   let cellProdId = row.insertCell(2);
   let cellNumEvents = row.insertCell(3);
   let cellNumProps = row.insertCell(4);

   cellName.innerHTML = "NEW CELL1";
   cellVer.innerHTML = "NEW CELL2";
   cellProdId.innerHTML = "NEW CELL3";
   cellNumEvents.innerHTML = "NEW CELL4 \n newCekkt";
   cellNumProps.innerHTML = "NEW CELL5";

}

function addText(){
  let div = document.getElementById('statusDiv');
  let val = $('#file-upload')[0].value;
  console.log(val);
  div.innerHTML += val.toString() + '<br />';
}


function clearText(divID){
  let div = document.getElementById(divID);

  div.innerHTML = '';

  return false;
}

// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    //On page-load AJAX Example


    $.ajax({
        type: 'get',            //Request type
        url: '/uploads/',   //The server endpoint we are connecting to
        success: function (data) {
          console.log('data:' + data);
          let html ='';
          let name = '';
          let table = document.getElementById("fileTable");
          let dropdown = document.getElementById("ddFiles");
          if(data.length == 0){
            document.getElementById('fileForm').innerHTML = 'No files';
          }else{
            document.getElementById("fileTable").style.visibility = "visible";
          }

          $.each(data, function(){
            name = this.toString();
            if(name.match('.ics')){
              let row = table.insertRow(-1);
              let cellName = row.insertCell(0);
              cellName.innerHTML = '<a href="/uploads/'+ this + '">'+this+'</a>';
              $(dropdown).append( '<li><a class="dropdown-item" href="#">' + this + '</a></li>' );
            }

          });



        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });



    //$('#file-upload')[0].value
    // Event listener form replacement example, building a Single-Page-App, no redirects if possible
    $('#upload-select').click(function(){
        let val = document.querySelector('#file-upload').files[0];
        console.log(val);
        //Pass data to the Ajax call, so it gets passed to the
        //
        ajax({
          type: 'post',
          data: 'hello',
          url:'/upload',
          dataType: 'calendar',
          success: function (data) {
            console.log('ok');
          },
          fail: function(error) {
              // Non-200 return, do something with error
              console.log('ERROR:');
              console.log(error);
          }
        });

    });

});
