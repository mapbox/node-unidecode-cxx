var http = require('http');
var unidecode = require('unidecode');
var unidecodeCxx = require('./unidecode');
var fs = require('fs');

var iterations = 100;

var Decoder = new unidecodeCxx.Decoder();

function compare(data) {
    lines = data.split("\n");

    console.time("unidecode");
    for (var i = 0; i < iterations; i++) {
        for (var j = 0; j < lines.length; j++) {
            unidecode(lines[j]);
        }
    }
    console.timeEnd("unidecode");


    console.time("cxx decodable");
    for (var i = 0; i < iterations; i++) {
        for (var j = 0; j < lines.length; j++) {
            unidecodeCxx.decodable(lines[j]);
        }
    }
    console.timeEnd("cxx decodable");


    console.time("cxx decode");
    for (var i = 0; i < iterations; i++) {
        for (var j = 0; j < lines.length; j++) {
            unidecodeCxx.decode(lines[j]);
        }
    }
    console.timeEnd("cxx decode");

    console.time("cxx decode2");
    for (var i = 0; i < iterations; i++) {
        for (var j = 0; j < lines.length; j++) {
            Decoder.decode(lines[j]);
        }
    }
    console.timeEnd("cxx decode2");
}

if (fs.existsSync('34013-0.txt')) {
    compare(fs.readFileSync('34013-0.txt').toString());
} else {
    console.log("Retrieving file...");
    http.get("http://mapbox.s3.amazonaws.com/apendleton/34013-0.txt", function(response) {
        var body = '';
        response.on('data', function (chunk) {
            body += chunk;
        });
        response.on('end', function () {
            console.log("Done.")
            compare(body);
        });
    });
}
