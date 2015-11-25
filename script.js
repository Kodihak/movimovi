var url, data;
var id = 1;
var MAX_COUNT = 100000;
var MAX_COUNT_PERCENT = MAX_COUNT / 100;
var count = 0;
var batchCount = 0;
var expectedCount = 0;
var movies = new Array();
var startTime, endTime;
var cumulativeTime = 0;
var TOTAL_MOVIES = 3500000;

function pad(n, z) {
    z = z || '0';
    n = n + '';
    return n.length >= 7 ? n : new Array(7 - n.length + 1).join(z) + n;
}

function sendRequests() {
    for (var i = 0; i < MAX_COUNT_PERCENT; i++) {
        url = 'http://www.omdbapi.com/?i=tt' + pad(id) + '&r=json';

        $.ajax(url, {
            success: function (result, status, xhr) {
                expectedCount++;
            },
            error: function (xhr, status, error) {
                expectedCount++;
            },
            complete: function (xhr, status) {
                if (status === "success") {
                    data = $.parseJSON(xhr.responseText);

                    var titleIsValidMovie = (typeof (data.Title) != "undefined");
                    titleIsValidMovie &= data.Type === "movie";
                    titleIsValidMovie &= (data.Rated === "R" || data.Rated === "PG-13" || data.Rated === "PG" || data.Rated === "G");

                    if (typeof (data.imdbVotes) != "undefined") {
                        titleIsValidMovie &= (parseFloat(data.imdbVotes.replace(/,/g, '')) >= 2000);
                    } else {
                        titleIsValidMovie = false;
                    }

                    if (titleIsValidMovie) {
                        movies.push(data);
                    }
                }
            },
            error: function (xhr, status) {

            }
        });

        id++;
    }
}

$(document).ajaxComplete(function (event, xhr, settings) {
    count++;
    batchCount++;

    // We have sent 1000 requests
    if (expectedCount == MAX_COUNT_PERCENT) {
        if (batchCount >= 950) {
            expectedCount = 0;
            batchCount = 0;
            window.setTimeout(sendRequests, 5000);
        }
    }

    if (count == 1) {
        console.log("Beginning search...");
        startTime = Date.now();
    } else if (count % MAX_COUNT_PERCENT == 0 && count != MAX_COUNT) {
        var percent = count / MAX_COUNT_PERCENT;
        var time = Date.now() - startTime;
        cumulativeTime += time;
        startTime = Date.now();
        var seconds = (time / 1000).toFixed(3);
        var x = 'X'.repeat(percent);
        var dot = '.'.repeat(100 - percent);
        console.log(percent + '% [' + x + dot + ']   Seconds passed: ' + seconds + ' Cumulative time: ' + (cumulativeTime / 1000).toFixed(3) + ' Average seconds: ' + (cumulativeTime / percent / 1000).toFixed(3));
    } else if (count == MAX_COUNT) {
        endTime = Date.now();
        var time = endTime - startTime;
        console.log("Processed " + MAX_COUNT + " movies in " + (time / 1000).toFixed(2) + " seconds.");
        console.log("Found " + movies.length + " viable movies.");
        $('#response').html(JSON.stringify(movies));
    }
});

function start() {
    sendRequests();
}