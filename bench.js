var http = require('http');
var unidecode = require('unidecode');
var unidecodeCxx = require('./unidecode');

function compare(data) {
    lines = data.split("\n");

    console.time("unidecode");
    for (var i = 0; i < 100; i++) {
        for (var j = 0; j < lines.length; j++) {
            unidecode(lines[j]);
        }
    }
    console.timeEnd("unidecode");

    console.time("unidecodeCxx");
    for (var i = 0; i < 100; i++) {
        for (var j = 0; j < lines.length; j++) {
            unidecodeCxx(lines[j]);
        }
    }
    console.timeEnd("unidecodeCxx");
}

console.log("Retrieving file...");
http.get("http://www.gutenberg.org/files/34013/34013-0.txt", function(response) {
    var body = '';
    response.on('data', function (chunk) {
        body += chunk;
    });
    response.on('end', function () {
        console.log("Done.")
        compare(body);
    });
});