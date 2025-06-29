#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
bool ValidateXML(char* xml, char* xsd);



typedef struct {
    char* homepage;
    char* appName;
    char* version;
    char guid[36 + 1];
    char* license;
    char* description;
}XmlManifest_AppNode;

typedef struct {
    char* os;
    char* arch;
}XmlManifest_PltfNode;

typedef struct{
    char type[32];
    char src[128];
    char name[1024];
    char desc[256];
    //linked list here
}_XmlDepNodeGnrl;

typedef struct {
    _XmlDepNodeGnrl* runtime;
    _XmlDepNodeGnrl* optional;
    _XmlDepNodeGnrl* conflicts;
    //linked lis there
}XmlManifest_DepNode;

typedef struct {
    char* authorType;
    char name[64];
    char portalID[36+1];
    char* contactType;
    char* contactSrc;
    //linked list here

}XMLManifest_AuthorNode;

typedef struct {
    uint8_t manifestVersion;
    XmlManifest_AppNode* appNode;
    XmlManifest_PltfNode* pltfNode;
    uint8_t versionSlot;
    XmlManifest_DepNode* depNode;
    XMLManifest_AuthorNode* authors;

}XmlManifestRoot;

void ProcessXMLNode(xmlNode* node, int depth);