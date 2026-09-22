
For football game results the OverviewPage shall display the results of one game day, consisting of the results
of multiple football matches. 

The result are to be displayed in a SilicaFlickable. Each Item in the flickable contains one result. 
The result consists of the data of the home team (name, logo) and the data of the away team (name, logo) and the result
of the game. Also the date of the game is to be displayed. 

Center the logo and the name of the home away team in the middle of the column. Use only the default SFOS margins.
Make sure that the home and away team get the same width, each 50% of the width.

Create a separate component for the flickable items that display a single result.

Put the components to the components directory.

Add a Settings page, that lets the user allow to configure a competition id (String). The input field is to be labeled properly.
The settings page can be reached via the pully menu from the Overview Page.
User Nemo.Configuration to store the compeition id using a ConfigurationGroup in the ApplicationWindow.
When the SettingsPage is left, do persist the configured compeition id.

Use mock data for 10 made up matches, which are then displayed in the page. Do not use a List model with ListElement, but
create a javascript model. Do use a SilicaListView with delegate ListItem instead of a Repeater.

Add the fontdecoder in c++ (with all dependencies) from the tools/font_obfuscation directory to the SailfishOS project 
under the src directory. Create a new directory fontobfuscation in the src directory.

Add new header file constants.h to the project defining the endpoint for the game results. The url is (for match day 2 is)
https://next.fussball.de/_next/data/3GE9wzufdQY_tRu-Qkpfs/de/widget/competition/ac5e71aa-1ee8-4579-9508-1a2f27fe240d/spieltag/2.json

Add method getMatchDay(int) to the FussballBackend that calls the endpoint for the match results and setts the proper match day.
The JSON result provided by the endpoint is then to be obfuscated, using the font decoder class. For an example how to apply
the font decoding use the main.cpp in tools/font_obfuscation as reference. The decoded json is the emitted as result. 
Add a Connections element to the OverviwPage to receive the result of the getMatchDay method. 

Call the get getMatchDay method in the OverviewPage to get the data and log the response to the console.
Additionally replace the mock data previously defined with the actual game results from the service call.

The currently selected match day is also stored in the ConfigurationGroup of the ApplicationWindow as currentMatchDay, by
default it is 0 (match day index starts with 0).

The OverviewPage has two additional pully menu entries. One to navigate to the previous match day. One to navigate to the next
match day. When the pully is selected the updated match day is to be set and stored in the configuration. Additionally the data
for the match day is to be loaded and displayed. The maximum number of the match day can be extracted from the json response.
If the current match day is 0, the pully for the previous match day is not to be displayed. If the current match day is the last 
match day, the next match day pully is not to be displayed, so we cannot violate the match day boundaries.



