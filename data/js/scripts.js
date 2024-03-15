var timerRunning, timeLeft;

function update_ActualTemp() {

    setInterval(getActualTemp, 500);
}


function getActualTemp() {
    $.get("/getActualTemp", function (data) {
        $("#actualTemp").html(data);
        actualTemp_set = parseInt(data);
    })
};

function hourSelector() {
    //console.log("hour_set " + hour_set)
    var optionsHour = "<option  value='-1' >Select Hours</option>";
    for (i = 0; i < 7; i++) {
        if (hour_set === i) {

            optionsHour = optionsHour + "<option value=" + i + " selected>" + i + "</option>"
        }
        else {

            optionsHour = optionsHour + "<option value=" + i + " >" + i + "</option>"
        }
    }

    //$("#hourSelector").css( "border", "3px solid red" );
    $("#hourSelector").html("<select class='form-select' aria-label='Default select example' name='hour_select' id='hour_select'>" + optionsHour + "</select>");
};

function minuteSelector() {
    //console.log("minute_set " + minute_set);
    var optionsMinute = "<option selected value='-1'>Select Minutes</option>";
    for (i = 0; i < 60; i++) {
        if (minute_set === i) {
            //   console.log(i + " minute_set " + "true")
            optionsMinute = optionsMinute + "<option value=" + i + " selected>" + i + "</option>"
        }
        else {
            //  console.log(i + " minute_set " + "false")
            optionsMinute = optionsMinute + "<option value=" + i + " >" + i + "</option>"
        }

        // $("#minuteSelector").css( "border", "3px solid red" );
        $("#minuteSelector").html("<select class='form-select' aria-label='Default select example' name='minute_select' id='minute_select'>" + optionsMinute + "</select>");
    }
}
function getHour_set() {
    $.get("/getHours", function (data) {
        $("#hoursDisplay").html(data);
        hour_set = parseInt(data);
        hourSelector();
    })

};

function getMinute_set() {
    $.get("/getMinutes", function (data) {
        $("#minutesDisplay").html(data);
        minute_set = parseInt(data);
        minuteSelector();
    })

};

function areYouThere() {
    $.get("AreYouThere", function (data) {
        if(data="Yes"){
            $("#onlineIndicator").css({"background-color":"green","width":"100px","height":"100px", "border-radius": "50px"});
        }else
        {
            $
        }

    })
    .fail(function(data){
        $("#onlineIndicator").css({"background-color":"red","width":"100px","height":"100px", "border-radius": "50px"});
    })
    .done(function (data) {

    })
}

function gettargetTemp() {
    $.get("gettargetTemp", function (data) {
        $("#targetTemp").html(data);
        $("#targetTemp_select[value]").val(data);
    }).done(function (data) {

    })
}

function post_set_targetTemp() {
    var targetTemp_select = $("#targetTemp_select").val();
    $.post("/setTargetTemp", {
        targetTemp_select: targetTemp_select
    })
        .done(function (data) {
            console.log(data);
            gettargetTemp();

        })


}
function post_set_timer() {
    var hours_select = $("#hour_select").val();
    var minutes_select = $("#minute_select").val();


    $.post("/setTimer", {
        hours_select: hours_select,
        minutes_select: minutes_select


    })
        .done(function (msg) {
            getHour_set();
            getMinute_set();

        })

}

function post_timeLeft() {
    if (timerRunning == 1) {
        var timeLeft = $("#timeLeftDisplay").html();
        $.post("/setTimeLeft", {
            timeLeftVal: timeLeft
        })
            .done(function (msg) {
                //  console.log(msg);
            })
    }
    //  console.log(timeLeft);
}


function currentDateTime() {
    var currentDateTimeString = new Date().toLocaleString();

    $("#currentDateTimeDisplay").html(currentDateTimeString);

}

function startTimer() {
   // console.log("startTimer()");
    timeLeftInterval = setInterval(post_timeLeft, 10000);
    timerRunning = 1;
 //   console.log(timerRunning);
    $.post("/setTimerRunning", { timerRunningVal: String(timerRunning) })
        .done(function (data) {
            //alert( " success" );
        //    console.log(data + " data");
            timerRunning = 1;
            startTime = Date.now();
       //     console.log("startTime " + startTime);
            $("#timerRunningDisplay").html(timerRunning);
        //    console.log(timerRunning + " timerRunning");

            $.post("/setStartTime", { StartTimeVal: startTime })
                .done(function () {
                    startTimeString = new Date(startTime).toLocaleString();
                    $("#startDateTimeDisplay").html(startTimeString);
        //            console.log(startTimeString);
                    stopTime = startTime + (hour_set * 3600 * 1000) + (minute_set * 60 * 1000);
                    timeLeft = stopTime - startTime;
                    console.log("stopTime " + stopTime);
                    console.log("timeLeft " + timeLeft);
                    $.post("/setStopTime", { StopTimeVal: stopTime })
                        .done(
                            function () {
                                stopTimeString = new Date(stopTime).toLocaleString()
                                $("#stopDateTimeDisplay").html(stopTimeString);
            //                    console.log("timeLeft " + timeLeft);
                                $.post("/setTimeLeft", { timeLeftVal: timeLeft })
                                    .done(function () { $("#timeLeftDisplay").html(timeLeft); })
                            })

                }
                )
        })
        .fail(function () {
            alert("error");
        });


}

function getTimerRunning() {

    return $.get("/getTimerRunning", function (data) {

        $("#timerRunningDisplay").html(parseInt(data));
        //       console.log("timerrunning")
        //       console.log(data)
    })


}

function setTimeLeft() {
//console.log("pre stopTime" + stopTime);
    return $.get("/getStopTime", function (data) {

        timerRunning = parseInt($("#timerRunningDisplay").html());
        //console.log(timerRunning);
        var stopTime_int = parseInt(data);
        //  console.log(stopTime_int);
        var stopTime = new Date(stopTime_int).toLocaleString();
        //console.log(timeLeft);

        if (stopTime_int == 0) {
            $("#stopDateTimeDisplay").html("Not Set");
            $("#timeLeftDisplay").html("0");
        }
        else {
            $("#stopDateTimeDisplay").html(stopTime);
        }
        if (timerRunning == 1) {
            var currentDateTime = Date.now();
            timeLeft = stopTime_int - currentDateTime;

            $("#timeLeftDisplay").html(timeLeft);
        }



        if (timerRunning == 1 && typeof stopTime_int != "undefined") {
            if (timeLeft <= 0) {
                stopTimer();
                $("#timeLeftDisplay").html("0");
                // clearInterval(countdownInterval);
                clearInterval(timeLeftInterval);
            } else {
                $("#timeLeftDisplay").html(timeLeft);
            }

        }

    }
    )

};

function stopTimer() {

   // console.log("STOP");
    timerRunning = 0;
    $("#timerRunningDisplay").html(timerRunning);
    var timerRunningStop = 0;
    timeLeft = 0;

    $.post("/setTimerRunning", { timerRunningVal: timerRunningStop })
        .done(function () {
            // alert( "second success" );
            $.post("/setStartTime", { StartTimeVal: 0 });
            $.post("/setStopTime", { StopTimeVal: 0 })
            $.post("/setTimeLeft", { timeLeftVal: 0 })
            $("#startDateTimeDisplay").html("Not Set");
            $("#stopDateTimeDisplay").html("Not Set");
            $("#timeLeftDisplay").html("0");
            $("#clockValueDisplay").html("00:00:00");
            //clearInterval(countdownInterval);
            clearInterval(timeLeftInterval);
        })
        .fail(function () {
            //alert("error");
        });

}

function getTimeLeft() {

    console.log(timeLeft);
    $.get("/getTimeLeft", function (data) {
        console.log(data);
        timeLeft = parseInt(data);
        $("#timeLeftDisplay").html(timeLeft);

    })
    console.log(timeLeft);
};

function getStartTime() {
    var dfd = jQuery.Deferred();
    return $.get("/getStartTime", function (data) {
        //console.log("getstarttime");
        startTime_int = parseInt(data);

        startTime = new Date(startTime_int).toLocaleString();

        if (startTime_int == 0) {
            $("#startDateTimeDisplay").html("Not Set");
            $("#timeLeftDisplay").html("0");
        }
        else {
            $("#startDateTimeDisplay").html(startTime);
        }


    })

}

function getStopTime() {
    var dfd = jQuery.Deferred();
    return $.get("/getStopTime", function (data) {
        //        console.log("getsstoptime");
        stopTime_int = parseInt(data);
        //console.log(stopTime_int);
        stopTime = new Date(stopTime_int).toLocaleString();

        if (stopTime_int == 0) {
            $("#stopDateTimeDisplay").html("Not Set");
            $("#timeLeftDisplay").html("0");
        }
        else {
            $("#stopDateTimeDisplay").html(stopTime);
        }
    })
    return dfd.promise();
}

function countDownTimer() {
    //  console.log("countDownTimer");
    timerRunning = $("#timerRunningDisplay").html();

    clockDisplay(timeLeft);

    currentDateTime();

    if (timerRunning == 1 && timeLeft > 0) {
        setTimeLeft();
    }
    else if (timerRunning <= 0) {
        stopTimer();
    }

}

function clockDisplay(timeLeft) {

    var hoursLeft = parseInt(timeLeft / (3600 * 1000));

    var minutesLeft = parseInt((timeLeft - hoursLeft * (3600 * 1000)) / (60 * 1000));

    var secondsLeft = parseInt(((timeLeft - hoursLeft * (3600 * 1000)) - (minutesLeft * 60 * 1000)) / 1000);


    var clockValueString;
    clockValueString = addLeadingZero(hoursLeft) + ":" + addLeadingZero(minutesLeft) + ":" + addLeadingZero(secondsLeft);

    $("#clockValueDisplay").html(clockValueString);
}
function addLeadingZero(value) {
    var output_str;
    if (value < 10) {
        output_str = "0" + String(value);
    }
    else {
        output_str = String(value);
    }
    return output_str;
}

function startAlarm() {
    alarm_int = 1;
    $.post("/setAlarm", {
        Alarm_int: alarm_int
    })
        .done()
}

function stopAlarm() {
    alarm_int = 0;
    $.post("/setAlarm", {
        Alarm_int: alarm_int
    })
        .done()
}

function pid_action() {
    timer_running_val = 1;
    var kp_input = $("#kp_input").val();
    var ki_input = $("#ki_input").val();
    var kd_input = $("#kd_input").val();
  
    $.post("/setPID", {
      pid_kpValue: kp_input,
      pid_kiValue: ki_input,
      pid_kdValue: kd_input
    })
      .done(function (msg) {
        console.log(msg);
  
  
      })
  
  
  
  
  };

  function getpid_kp() {
    $.get("/getKp_set", function (data) {
   //      console.log(data);
      pid_kp = data;
      $("#kp_input[value]").val(pid_kp);
    })
  }
  function getpid_ki() {
    $.get("/getKi_set", function (data) {
  //      console.log(data);
      pid_ki = data;
      $("#ki_input[value]").val(pid_ki);
    })
  }
  
  function getpid_kd() {
    $.get("/getKd_set", function (data) {
  //        console.log(data);
      pid_kd = data;
      $("#kd_input[value]").val(pid_kd);
    })
  }
  function updateKpInput() {
    getpid_kp();
    //  console.log("targetTemp_set " + targetTemp_set);
    //$("#targetTempInput[value]").val(pid_kp);
  }
  
  function updateKiInput() {
    getpid_ki();
    //$("#targetTempInput[value]").val(pid_ki);
  }
  
  function updateKdInput() {
    getpid_kd();
    // $("#targetTempInput[value]").val(pid_kd);
  }

  function getPIDoutput() {
    $.get("/PID_Output", function (data) {
   //     console.log(data);
      $("#output").html(data);
    })
    // $("#targetTempInput[value]").val(pid_kd);
  }

  function setHeaterInterval() {
    heaterInterval = setInterval(get_heater_state, 100);
  };

  function get_heater_state() {
    getPIDoutput();
    $.get("/get_heater_state", function (data) {
  
      heater_state = parseInt(data);
 // console.log(heater_state)
      if (heater_state == 1) {
  
        $("#heatingon").show();
        $("#heatingoff").hide();
  
      } else {
  
        $("#heatingon").hide();
        $("#heatingoff").show();
      }
    }
    )
  }
  function setHeaterInterval() {
    heaterInterval = setInterval(get_heater_state, 100);
  };

  function restartEsp(){
    $.get("/ESPrestart");
    //setTimeout(location.reload(),200000);
    setTimeout(function(){location.reload()},7000);
   
  }