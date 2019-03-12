
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

  div.innerHTML += 'Extra stuff ';

  return false;
}

function clearText(divID){
  let div = document.getElementById(divID);

  div.innerHTML = 'clear';

  return false;
}

// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    //On page-load AJAX Example


    $.ajax({
        type: 'get',            //Request type
        url: '/uploads/',   //The server endpoint we are connecting to
        success: function (data) {
          let tableContent ='';
          let table = document.getElementById("fileTable");

          $.each(data, function(){
            let row = table.insertRow(-1);
            let cellName = row.insertCell(0);
            cellName.innerHTML = this;

          });
          console.log(tableContent);

        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });
    //
    //
    //
    //
    // // Event listener form replacement example, building a Single-Page-App, no redirects if possible
    // $('#someform').submit(function(e){
    //     $('#blah').html("Callback from the form");
    //     e.preventDefault();
    //     //Pass data to the Ajax call, so it gets passed to the
    //     $.ajax({});
    // });

});
