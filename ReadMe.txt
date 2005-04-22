DDE Query v1.5
==============

1) Installation.

Run Setup.exe or copy the file DDEQuery.exe to a folder.


2) Uninstallation.

Delete the files DDEQuery.*, ReadMe.txt and RelNotes.txt.


3) Manual.

This is a utility for querying DDE servers. It has support for all the synchronous
transaction types - Request, Advise, Execute and Poke.

Start by opening a conversation to the DDE Server using "Server | Connect...". The
dialog uses XTYP_WILDCONNECT to find all available DDE Servers and their supported
topics. If the DDE server is not listed it probably does not support
XTYP_WILDCONNECT. If you have a "Link" on the clipboard you can use the
"Paste Link" button to set the Service and Topic to those referenced in the link.

Once a conversation is established you can then enter an Item name and select a
clipboard format in the "Command Parameters" section and request the Item using
"Command | Request Item" or establish an advise loop (link) to it using
"Link | Start Advise". If you requested the value, it will appear in the Value
box below the Item name; if you linked to it, a link will appear in the "Active
Links" section and update when the DDE server sends an advise (it always creates
a "hot" link). The clipboard format combo has a few common choices in it, but
you can also type the name of other standard formats, e.g. "CF_DIBV5" or you can
enter the name of a custom format, e.g. "XlTable", "Biff8", "HTML Format" etc.
By default all custom formats are treated as binary.

In the "Value" box and "Active Links" view there is only room to display a small
amount of the actual DDE value, and for binary formats you will only see the size
of the data recieved, e.g. "(20 bytes)". To display the entire value use the
"Full Value" button (or double-click a link) and a resizable dialog will popup
with the entire value rendered as ANSI text and separate view showing the value
as lines of hex values.

When an advise is triggered for a link a small icon will display for a short
period of time to the left of the item name to notify you of the update. A
similar icon will appear in the status bar when any DDE call is in progress
to indicate that DDE Query is waiting on the DDE server.

For an XTYP_POKE enter the item name, select a clipboard format and enter the
data in the Value box. For an XTYP_EXECUTE you only need to enter the DDE
command in the Item box. Then select the appropriate "Command | ..." menu entry.

Instead of entering links manually you can create a text file with the item names
and use "Link | Open File..." to read the file and create them. The file requires
one item per line and they are all requested as CF_TEXT. Empty lines and those
starting with a '#' are treated as comments and ignored, e.g.

# Spot Rates (REUTER|IDN)
GBP=,[BID,ASK]
EUR=,[BID,ASK]

If you have manually added a set of links and want to keep them, you can use
"Link | Save File..." which will generate a text file with the link item names
in it, for use with "Link | Open File..." later.

If the DDE server you have a conversation with only sends updates through, not
an initial value when you create an advise loop, e.g. Excel, you can request the
current values for all unadvised links with the "Link | Request Values" menu
entry. 

The "Link" commands "Copy To Clipboard", "Copy As Item" and "Paste As Item"
allow you to use the Copy and Paste features of an application that understands
the "Link" clipboard format to save manually typing item names. When copying a
link to the clipboard the entire link, e.g. "SERVICE|TOPIC!ITEM", is generated,
but when pasting, only the "ITEM" name is extracted as it is assumed to be valid
for the currently open conversation.


4) Contact Details.

Email: gort@cix.co.uk
Web:   http://www.cix.co.uk/~gort


Chris Oldwood 
23rd April 2005
