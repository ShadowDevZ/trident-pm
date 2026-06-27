#pragma once

#include "datatypes.h"
#include <optional>
#include "trderr.h"
#include "dynamicTable.h"
/*
this will be the most complex part of the whole format and WILL be rewritten multiple times
because it will be bug infested mess if we want things as in place operations and such in future

TREG will be a metadata storage (key-value) for each dtbl directory entry
featuring simplistic filesystem like directories/entries, modifiable attributes,
dynamic fields and table specific information that will be embedded most likely as XATTR attribute
byte array that will be handled by each TABLE differently if needed

the binary data layout will look something like this 
[treg header RHDR (few bytes)]
[byte array dump] CELL_TYPE (KEY,VALUE,ATTRIBUTER),
                  CELL_INFO CELL_PARSING_REQUIRED_FLAG
SIZEOF(CELL), CELLDATA
if parser doesnt know what is some certain table doing they can just currentOffset += cellSize

cell data info varies depending whether its K,V, or A, they share in common the CELL_INFO struct
each entry also has internal CRC32 validation. Also the cell data contains attribute on child nodes.

The first version will probably be immutable meaning that to rewrite or update the data we have to
firsly read the data then modify in memory then write at offset if we are adding fields we need
to do COW the entire treg. In future i definitely want windows like registry where data can be added/deleted
without copying to the new file and deleting certain tables, finding holes (cellsize negative if free like some fs do it)

also we need an interface for CELLDATA tables so the tables are required to fill the basic info
*/