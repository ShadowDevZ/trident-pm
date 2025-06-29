#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include <stdbool.h>
#include "trderr.h"
#include "xmlparse.h"
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
    bool status = false;
    if (res == 0) {
        printf("XML is valid against XSD");
        status = true;
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
    
    
    return status;
}
bool _IsContainerXMLNode(xmlNode* node) {
    for (xmlNode* child = node->children; child; child = child->next) {
        if (child->type == XML_ELEMENT_NODE) {
            return true;
        }
    }
    return false;
}
//todo use XmlManifestRoot
void ProcessXMLNode(xmlNode* node, int depth) {
    for (xmlNode* cNode = node; cNode; cNode = cNode->next) {
        if (cNode->type != XML_ELEMENT_NODE) continue;

        xmlChar* nodeContent = xmlNodeGetContent(cNode);
        bool isContainerNode = _IsContainerXMLNode(cNode);

        
        for (int i = 0; i < depth; ++i)  {
            printf("  ");
        }
        printf("Node: %s\n", cNode->name);

        
        if (nodeContent && xmlStrlen(nodeContent) > 0 && !isContainerNode) {
            for (int i = 0; i < depth; ++i)  {
                printf("  ");
            }
            printf("  Value: %s\n", nodeContent);
        }

        // Print attributes
        if (cNode->properties) {
            for (int i = 0; i < depth; ++i) printf("  ");
            printf("  Attributes:");
            for (xmlAttr* attr = cNode->properties; attr; attr = attr->next) {
                xmlChar* prop = xmlGetProp(cNode, attr->name);
                if (prop) {
                    printf(" %s=\"%s\"", attr->name, prop);
                    xmlFree(prop);
                }
            }
            printf("\n");
        } else {
            for (int i = 0; i < depth; ++i)  {
                printf("  ");
            }
            printf("  Attributes: N/A\n");
        }

        xmlFree(nodeContent);

        // Recurse into children
        ProcessXMLNode(cNode->children, depth + 1);

        
        printf("\n");
        
    }
}
