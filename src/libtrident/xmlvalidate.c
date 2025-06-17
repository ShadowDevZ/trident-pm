#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include <stdbool.h>
#include "trderr.h"
bool ValidateXML(char* xml, char* xsd) {
    //xmlInitParser();
    if (!CheckFile(xml, "") || !CheckFile(xsd, "")) {
        return false;
    }
    if (!strcmp(xml, xsd)) {
        return false;
    }
    xmlInitParser();
    xmlSchemaParserCtxt* schemaCtx = xmlSchemaNewParserCtxt(xsd);
    if (!schemaCtx) {
        printf("Failed to create xsd parser context\n");
        return false;
    }
    xmlSchema* schema = xmlSchemaParse(schemaCtx);
    if (!schema) {
        printf("Could not parse XSD schema\n");
        xmlSchemaFreeParserCtxt(schemaCtx);
        return false;
    }
    xmlSchemaFreeParserCtxt(schemaCtx);
    xmlDoc* xsdDoc = xmlReadFile(xsd, NULL, 0);
    if (!xsdDoc) {
        printf("Failed to read XSD\n");
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(schemaCtx);
        return false;
        
    }
    xmlDoc* xmlDoc = xmlReadFile(xml, NULL, 0);
    if (!xmlDoc) {
        printf("Failed to read XML\n");
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(schemaCtx);
        return false;
    }

    xmlSchemaValidCtxt* validationCtx = xmlSchemaNewValidCtxt(schema);
    if (!validationCtx) {
        printf("Could not create XSD validation context\n");
        xmlFreeDoc(xmlDoc);
        xmlSchemaFree(schema);
        return false;
    }
    int res = xmlSchemaValidateDoc(validationCtx, xmlDoc);
    if (res == 0) {
        printf("XML is valid against XSD");
    }
    else if (res > 0) {
        printf("XML is not valid against XSD");
    }
    else {
        printf("Internal lbixml2 parser error\n");
    }
    xmlSchemaFreeValidCtxt(validationCtx);
    xmlFreeDoc(xmlDoc);
    xmlFreeDoc(xsdDoc);
    xmlSchemaFree(schema);
    xmlCleanupParser();
    
    
    return true;
    }