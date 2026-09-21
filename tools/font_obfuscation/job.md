Reverse engieer the font obfuscation used by fussball.de. The endponit https://next.fussball.de/_next/data/3GE9wzufdQY_tRu-Qkpfs/de/widget/competition/ac5e71aa-1ee8-4579-9508-1a2f27fe240d/spieltag/1.json provides a json that uses font obfuscation in some parts of the data. do reverse engineer the font obfuscation. the font can be found
under this url https://www.fussball.de/export.fontface/-/format/woff/id/pvqsf4je/type/font

Split the logic into two pyhton scripts. One that does the font decoding and stores the decoded data in a file. 
And a second pyhton script that uses the data to apply the decoding on the json file. 

