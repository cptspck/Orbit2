var month;
var date;
var pubDict = {};



function getSchedule() {
	pubDict = {};
	
	var schedule;
	var starts = [];
	var ends = [];
	var names = [];
	var dict = {};
	var tempDict = [];
	var tempDict2 = [];
	var tempDict3 = [];

	var d = new Date();
	//d.setDate(19);
	//d.setHours(11);
	var dHour = new Date(d);
	var dNextHour = new Date(d);
	dNextHour.setHours(dNextHour.getHours() + 1);
	dNextHour.setMinutes(0);
	dHour.setMinutes(0);
	if(d.getMonth() < 10) {
		month = "0" + (d.getMonth() + 1).toString();
	} else {
		month = (d.getMonth + 1).toString();
	}

	if(d.getDate() < 10) {
		date = "0" + d.getDate().toString();
	} else {
		date = (d.getDate()+0).toString();
	}
	var dateString = d.getFullYear().toString() +  "-" + month + "-" + date;
	var url = "http://ion.tjhsst.edu/api/schedule/" + dateString;

	var xhttp = new XMLHttpRequest();
	xhttp.open("GET", url, false);
	xhttp.send();
	schedule = JSON.parse(xhttp.responseText);

	//console.log(url);
	var k = 0;
	// Build a dictionary.
	//console.log(schedule.date);
	if(schedule.date !== null) {
		for(var i = 0; i <= schedule.day_type.blocks.length - 1; i++){
			starts[i] = new Date(d);
			//console.log(starts[i]);
			starts[i].setHours(parseInt(schedule.day_type.blocks[i].start.split(":")[0]));
			parseInt(schedule.day_type.blocks[i].start.split(":")[0]);
			starts[i].setMinutes(parseInt(schedule.day_type.blocks[i].start.split(":")[1]));
			//console.log(starts[i]);
			ends[i] = new Date(d);
			ends[i].setHours(parseInt(schedule.day_type.blocks[i].end.split(":")[0]));
			ends[i].setMinutes(parseInt(schedule.day_type.blocks[i].end.split(":")[1]));
			//console.log(ends[i]);
			names[i] = schedule.day_type.blocks[i].name;
			//console.log(names[i]);
			if(starts[i] <= dNextHour && ends[i] >= dHour) {
				//console.log("-------------------");
				//console.log(dNextHour);
				//console.log(dHour);
				//console.log("its working");
				if(starts[i] <= dHour) {
					starts[i].setMinutes(0);
				}
				if(ends[i] >= dNextHour) {
					ends[i].setMinutes(59);
				}
				if(k === 0) {
					tempDict[0] = starts[i].getMinutes();
					tempDict[1] = ends[i].getMinutes();
					tempDict[2] = names[i];
					//console.log(tempDict[0] + "            " +tempDict[1]);
				}
				if(k == 1) {
					tempDict2[0] = starts[i].getMinutes();
					tempDict2[1] = ends[i].getMinutes();
					tempDict2[2] = names[i];
					//console.log(tempDict2[0] + "            " +tempDict2[1]);
				}
				if(k == 2) {
					tempDict3[0] = starts[i].getMinutes();
					tempDict3[1] = ends[i].getMinutes();
					tempDict3[2] = names[i];
					//console.log(tempDict3[0] + "            " +tempDict3[1]);
				}
				k ++;
			}
		}
	}
	//console.log(k);
	if (k == 1) {
		//console.log("k is 1");
		dict = {
			'Name1': tempDict[2],
			'End1': tempDict[1],
			'Start1': tempDict[0],
						
			'Name2':  '',
			'End2': '',
			'Start2':  '',
						
			'Name3': '',
			'End3': '',
			'Start3': '',
		};
	}
	if (k == 2) {
		//console.log("k is 2");
		dict = {
			'Name1': tempDict[2],
			'End1': tempDict[1],
			'Start1': tempDict[0],
						
			'Name2': tempDict2[2],
			'End2': tempDict2[1],
			'Start2': tempDict2[0],
						
			'Name3': '',
			'End3': '',
			'Start3': '',
		};	 
	}
	if(k == 3) {
		//console.log("k is 3");
		dict = {
			'Name1': tempDict[2],
			'End1': tempDict[1],
			'Start1': tempDict[0],
						
			'Name2': tempDict2[2],
			'End2': tempDict2[1],
			'Start2': tempDict2[0],
						
			'Name3': tempDict3[2],
			'End3': tempDict3[1],
			'Start3': tempDict3[0],
		};
	}
	// Send the object
	pubDict = dict;
   Pebble.sendAppMessage(dict, function() {
   //console.log('Message sent successfully from inside: ' + JSON.stringify(dict));
     }, function(e) {
      //console.log('Message failed: ' + JSON.stringify(e));
   });
} 

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    //console.log('PebbleKit JS ready!');
  // Update s_js_ready on watch
  Pebble.sendAppMessage({'JSReady': 1});
	 getSchedule();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    //console.log('AppMessage received!');
	  getSchedule();
  }                     
);



// Send the object
Pebble.sendAppMessage(pubDict, function() {
  //console.log('Message sent successfully: ' + JSON.stringify(pubDict));
}, function(e) {
  //console.log('Message failed: ' + JSON.stringify(e));
});
