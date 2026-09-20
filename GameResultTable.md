
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

Use mock data for 10 made up matches, which are then displayed in the page. 