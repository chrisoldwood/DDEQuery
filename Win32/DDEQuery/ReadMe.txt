DDE Query v1.0
==============

1) Installation.

Run Setup.exe or copy the file DDEQuery.exe to a folder.


2) Uninstallation.

Delete the files DDEQuery.*


3) Manual.

This is a utility for querying DDE servers. It has support for all the synchronous
transaction types - Request, Advise, Execute and Poke.

Start by opening a conversation to the DDE Server using "Server | Connect...". The
dialog uses XTYP_WILDCONNECT to find all available DDE Servers and their supported
topics. If the DDE server is not listed it probably does not support
XTYP_WILDCONNECT.

Once a conversation is established you can then enter an Item name and select a
clipboard format in the "command parameters" section and request the Item using
"Command | Request Item" or establish an advise loop (link) to it using
"Link | Start Advise". If you requested the value, it will appear in the Value
box below the Item name; if you linked to it, a link will appear in the "Active
Links" section and update when the DDE server sends an advise (it always creates
a "hot" link).

For an XTYP_POKE enter the item name, select a clipboard format and enter the
data in the Value box. For an XTYP_EXECUTE you only need to enter the DDE
command in the Item box. Then select the appropriate "Command | ..." menu entry.

To save entering links manually you can create a text file with the item names
in and use "Link | File..." to read the file and create them. The file requires
one item per line and they are all assumed to be CF_TEXT. Empty lines and those
starting with a '#' are treated as comments and ignored, e.g.

# Spot Rates (REUTER | IDN)
GBP=,BID ASK
EUR=,BID ASK


4) Contact Details.

EMail: gort@cix.co.uk
Web:   http://www.cix.co.uk/~gort


Chris Oldwood 
(gort@cix.co.uk)

22nd November 2004
