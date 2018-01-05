//
// TM & (c) 2017 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef MATERIALX_DOCUMENT
#define MATERIALX_DOCUMENT

/// @file
/// The top-level Document class

#include <MaterialXCore/Library.h>

#include <MaterialXCore/Look.h>
#include <MaterialXCore/Material.h>
#include <MaterialXCore/Node.h>

namespace MaterialX
{

extern const string DOCUMENT_VERSION_STRING;

/// A shared pointer to a Document
using DocumentPtr = shared_ptr<class Document>;
/// A shared pointer to a const Document
using ConstDocumentPtr = shared_ptr<const class Document>;

/// @class Document
/// A MaterialX document, which represents the top-level element in the
/// MaterialX ownership hierarchy.
///
/// Use the factory function createDocument() to create a Document instance.
class Document : public Element
{
  public:
    Document(ElementPtr parent, const string& name);
    virtual ~Document();

    /// Create a new document of the given subclass.
    template <class T> static shared_ptr<T> createDocument()
    {
        shared_ptr<T> doc = std::make_shared<T>(ElementPtr(), EMPTY_STRING);
        doc->initialize();
        return doc;
    }

    /// Initialize the document, removing any existing content.
    virtual void initialize();

    /// Create a deep copy of the document.
    virtual DocumentPtr copy()
    {
        DocumentPtr doc = createDocument<Document>();
        doc->copyContentFrom(getSelf(), true);
        return doc;
    }

    /// Import the given document as a library within this document.
    /// The contents of the library document are copied into this one, and
    /// are assigned the source URI of the library.
    /// @param library The library document to be imported.
    /// @param skipDuplicates If true then skip copying any child Elements with
    ///      if one with the same name already exists. Defaults to false.
    void importLibrary(ConstDocumentPtr library, bool skipDuplicates = false);

    /// @name Document Versions
    /// @{

    /// Set the version string of the document.
    void setVersionString(const string& version)
    {
        setAttribute(VERSION_ATTRIBUTE, version);
    }

    /// Return true if the given element has a version string.
    bool hasVersionString() const
    {
        return hasAttribute(VERSION_ATTRIBUTE);
    }

    /// Return the version string of the document.
    const string& getVersionString() const
    {
        return getAttribute(VERSION_ATTRIBUTE);
    }

    /// Return the major and minor versions as an integer pair.
    std::pair<int, int> getVersionIntegers();

    /// Upgrade the content of this document from earlier supported versions to
    /// the library version.  Documents from future versions are left unmodified.
    void upgradeVersion();

    /// @}
    /// @name NodeGraph Elements
    /// @{

    /// Add a NodeGraph to the document.
    /// @param name The name of the new NodeGraph.
    ///     If no name is specified, then a unique name will automatically be
    ///     generated.
    /// @return A shared pointer to the new NodeGraph.
    NodeGraphPtr addNodeGraph(const string& name = EMPTY_STRING)
    {
        return addChild<NodeGraph>(name);
    }

    /// Return the NodeGraph, if any, with the given name.
    NodeGraphPtr getNodeGraph(const string& name) const
    {
        return getChildOfType<NodeGraph>(name);
    }

    /// Return a vector of all NodeGraph elements in the document.
    vector<NodeGraphPtr> getNodeGraphs() const
    {
        return getChildrenOfType<NodeGraph>();
    }

    /// Remove the NodeGraph, if any, with the given name.
    void removeNodeGraph(const string& name)
    {
        removeChildOfType<NodeGraph>(name);
    }

    /// Return a vector of all port elements that match the given node name.
    /// Port elements support spatially-varying upstream connections to
    /// nodes, and include both Input and Output elements.
    vector<PortElementPtr> getMatchingPorts(const string& nodeName) const;

    /// @}
    /// @name Material Elements
    /// @{

    /// Add a Material to the document.
    /// @param name The name of the new Material.
    ///     If no name is specified, then a unique name will automatically be
    ///     generated.
    /// @return A shared pointer to the new Material.
    MaterialPtr addMaterial(const string& name = EMPTY_STRING)
    {
        return addChild<Material>(name);
    }

    /// Return the Material, if any, with the given name.
    MaterialPtr getMaterial(const string& name) const
    {
        return getChildOfType<Material>(name);
    }

    /// Return a vector of all Material elements in the document.
    vector<MaterialPtr> getMaterials() const
    {
        return getChildrenOfType<Material>();
    }

    /// Remove the Material, if any, with the given name.
    void removeMaterial(const string& name)
    {
        removeChildOfType<Material>(name);
    }

    /// @}
    /// @name GeomInfo Elements
    /// @{

    /// Add a GeomInfo to the document.
    /// @param name The name of the new GeomInfo.
    ///     If no name is specified, then a unique name will automatically be
    ///     generated.
    /// @param geom An optional geometry string for the GeomInfo.
    /// @return A shared pointer to the new GeomInfo.
    GeomInfoPtr addGeomInfo(const string& name = EMPTY_STRING, const string& geom = UNIVERSAL_GEOM_NAME)
    {
        GeomInfoPtr geomInfo = addChild<GeomInfo>(name);
        geomInfo->setGeom(geom);
        return geomInfo;
    }

    /// Return the GeomInfo, if any, with the given name.
    GeomInfoPtr getGeomInfo(const string& name) const
    {
        return getChildOfType<GeomInfo>(name);
    }

    /// Return a vector of all GeomInfo elements in the document.
    vector<GeomInfoPtr> getGeomInfos() const
    {
        return getChildrenOfType<GeomInfo>();
    }

    /// Remove the GeomInfo, if any, with the given name.
    void removeGeomInfo(const string& name)
    {
        removeChildOfType<GeomInfo>(name);
    }

    /// @}
    /// @name Look Elements
    /// @{

    /// Add a Look to the document.
    /// @param name The name of the new Look.
    ///     If no name is specified, then a unique name will automatically be
    ///     generated.
    /// @return A shared pointer to the new Look.
    LookPtr addLook(const string& name = EMPTY_STRING)
    {
        return addChild<Look>(name);
    }

    /// Return the Look, if any, with the given name.
    LookPtr getLook(const string& name) const
    {
        return getChildOfType<Look>(name);
    }

    /// Return a vector of all Look elements in the document.
    vector<LookPtr> getLooks() const
    {
        return getChildrenOfType<Look>();
    }

    /// Remove the Look, if any, with the given name.
    void removeLook(const string& name)
    {
        removeChildOfType<Look>(name);
    }

    /// @}
    /// @name Collection Elements
    /// @{

    /// Add a Collection to the document.
    /// @param name The name of the new Collection.
    ///     If no name is specified, then a unique name will automatically be
    ///     generated.
    /// @return A shared pointer to the new Collection.
    CollectionPtr addCollection(const string& name = EMPTY_STRING)
    {
        return addChild<Collection>(name);
    }

    /// Return the Collection, if any, with the given name.
    CollectionPtr getCollection(const string& name) const
    {
        return getChildOfType<Collection>(name);
    }

    /// Return a vector of all Collection elements in the document.
    vector<CollectionPtr> getCollections() const
    {
        return getChildrenOfType<Collection>();
    }

    /// Remove the Collection, if any, with the given name.
    void removeCollection(const string& name)
    {
        removeChildOfType<Collection>(name);
    }

    /// @}
    /// @name TypeDef Elements
    /// @{

    /// Add a TypeDef to the document.
    /// @param name The name of the new TypeDef.
    ///     If no name is specified, then a unique name will automatically be
    ///     generated.
    /// @return A shared pointer to the new TypeDef.
    TypeDefPtr addTypeDef(const string& name)
    {
        return addChild<TypeDef>(name);
    }

    /// Return the TypeDef, if any, with the given name.
    TypeDefPtr getTypeDef(const string& name) const
    {
        return getChildOfType<TypeDef>(name);
    }

    /// Return a vector of all TypeDef elements in the document.
    vector<TypeDefPtr> getTypeDefs() const
    {
        return getChildrenOfType<TypeDef>();
    }

    /// Remove the TypeDef, if any, with the given name.
    void removeTypeDef(const string& name)
    {
        removeChildOfType<TypeDef>(name);
    }

    /// @}
    /// @name NodeDef Elements
    /// @{

    /// Add a NodeDef to the document.
    /// @param name The name of the new NodeDef.
    ///     If no name is specified, then a unique name will automatically be
    ///     generated.
    /// @param type An optional type string.
    /// @param node An optional node string.
    /// @return A shared pointer to the new NodeDef.
    NodeDefPtr addNodeDef(const string& name = EMPTY_STRING,
                          const string& type = DEFAULT_TYPE_STRING,
                          const string& node = EMPTY_STRING)
    {
        NodeDefPtr child = addChild<NodeDef>(name);
        child->setType(type);
        if (!node.empty())
        {
            child->setNodeString(node);
        }
        return child;
    }

    /// Return the NodeDef, if any, with the given name.
    NodeDefPtr getNodeDef(const string& name) const
    {
        return getChildOfType<NodeDef>(name);
    }

    /// Return a vector of all NodeDef elements in the document.
    vector<NodeDefPtr> getNodeDefs() const
    {
        return getChildrenOfType<NodeDef>();
    }

    /// Remove the NodeDef, if any, with the given name.
    void removeNodeDef(const string& name)
    {
        removeChildOfType<NodeDef>(name);
    }

    /// Return a vector of all NodeDef elements that match the given node name.
    vector<NodeDefPtr> getMatchingNodeDefs(const string& nodeName) const;

    /// @}
    /// @name PropertySet Elements
    /// @{

    /// Add a PropertySet to the document.
    /// @param name The name of the new PropertySet.
    ///     If no name is specified, then a unique name will automatically be
    ///     generated.
    /// @return A shared pointer to the new PropertySet.
    PropertySetPtr addPropertySet(const string& name = EMPTY_STRING)
    {
        return addChild<PropertySet>(name);
    }

    /// Return the PropertySet, if any, with the given name.
    PropertySetPtr getPropertySet(const string& name) const
    {
        return getChildOfType<PropertySet>(name);
    }

    /// Return a vector of all PropertySet elements in the document.
    vector<PropertySetPtr> getPropertySets() const
    {
        return getChildrenOfType<PropertySet>();
    }

    /// Remove the PropertySet, if any, with the given name.
    void removePropertySet(const string& name)
    {
        removeChildOfType<PropertySet>(name);
    }

    /// @}
    /// @name Implementation Elements
    /// @{

    /// Add an Implementation to the document.
    /// @param name The name of the new Implementation.
    ///     If no name is specified, then a unique name will automatically be
    ///     generated.
    /// @return A shared pointer to the new Implementation.
    ImplementationPtr addImplementation(const string& name = EMPTY_STRING)
    {
        return addChild<Implementation>(name);
    }

    /// Return the Implementation, if any, with the given name.
    ImplementationPtr getImplementation(const string& name) const
    {
        return getChildOfType<Implementation>(name);
    }

    /// Return a vector of all Implementation elements in the document.
    vector<ImplementationPtr> getImplementations() const
    {
        return getChildrenOfType<Implementation>();
    }

    /// Remove the Implementation, if any, with the given name.
    void removeImplementation(const string& name)
    {
        removeChildOfType<Implementation>(name);
    }

    /// Return a vector of all node implementations that match the given
    /// NodeDef string.  Note that a node implementation may be either an
    /// Implementation element or NodeGraph element.
    vector<InterfaceElementPtr> getMatchingImplementations(const string& nodeDef) const;

    /// @}
    /// @name Public Elements
    /// @{

    /// Return any element with the given public name.  If multiple matches
    /// exist, then an arbitrary selection is made.
    ElementPtr getPublicElement(const string& publicName) const;

    /// Return a vector of all elements with the given public name.
    vector<ElementPtr> getPublicElements(const string& publicName) const;

    /// @}
    /// @name Require String
    /// @{

    /// Set the require string of the document.
    /// @todo Require strings are not yet automatically generated or validated
    ///    by the MaterialX library.
    void setRequireString(const string& require)
    {
        setAttribute(REQUIRE_ATTRIBUTE, require);
    }

    /// Return true if the document has a require string.
    bool hasRequireString() const
    {
        return hasAttribute(REQUIRE_ATTRIBUTE);
    }

    /// Return the require string of the document.
    const string& getRequireString() const
    {
        return getAttribute(REQUIRE_ATTRIBUTE);
    }

    /// Check the content of this document and store the appropriate require string.
    void generateRequireString();

    /// @}
    /// @name Color Management System
    /// @{

    /// Set the color management system string.
    void setColorManagementSystem(const string& cms)
    {
        setAttribute(CMS_ATTRIBUTE, cms);
    }

    /// Return true if a color management system string has been set.
    bool hasColorManagementSystem() const
    {
        return hasAttribute(CMS_ATTRIBUTE);
    }

    /// Return the color management system string.
    const string& getColorManagementSystem() const
    {
        return getAttribute(CMS_ATTRIBUTE);
    }

    /// @}
    /// @name Color Management Config
    /// @{

    /// Set the color management config string.
    void setColorManagementConfig(const string& cmsConfig)
    {
        setAttribute(CMS_CONFIG_ATTRIBUTE, cmsConfig);
    }

    /// Return true if a color management config string has been set.
    bool hasColorManagementConfig() const
    {
        return hasAttribute(CMS_CONFIG_ATTRIBUTE);
    }

    /// Return the color management config string.
    const string& getColorManagementConfig() const
    {
        return getAttribute(CMS_CONFIG_ATTRIBUTE);
    }

    /// @}
    /// @name Validation
    /// @{

    /// Validate that the given document is consistent with the MaterialX
    /// specification.
    /// @param message An optional output string, to which a description of
    ///    each error will be appended.
    /// @return True if the document passes all tests, false otherwise.
    /// @todo This method currently checks only a small subset of the MaterialX
    ///    rule set, and additional coverage is needed.
    bool validate(string* message = nullptr) const override;

    /// @}
    /// @name Callbacks
    /// @{

    /// Enable all observer notifications		
    virtual void enableNotifications() { }
    
    /// Disable all observer notifications		
    virtual void disableNotifications() { }

    /// Called when an element is added to the element tree.
    virtual void onAddElement(ElementPtr parent, ElementPtr elem);

    /// Called when an element is removed from the element tree.
    virtual void onRemoveElement(ElementPtr parent, ElementPtr elem);

    /// Called when an attribute of an element is set to a new value.
    virtual void onSetAttribute(ElementPtr elem, const string& attrib, const string& value);

    /// Called when an attribute of an element is removed.
    virtual void onRemoveAttribute(ElementPtr elem, const string& attrib);

    /// Called when a document is initialized.
    virtual void onInitialize() { }

    /// Called when data is read into the current document.
    virtual void onRead() { }

    /// Called when data is written from the current document.
    virtual void onWrite() { }

    /// Called before a set of document updates is performed.
    virtual void onBeginUpdate() { }

    /// Called after a set of document updates is performed.
    virtual void onEndUpdate() { }

    /// @}

  public:
    static const string CATEGORY;
    static const string VERSION_ATTRIBUTE;
    static const string REQUIRE_ATTRIBUTE;
    static const string CMS_ATTRIBUTE;
    static const string CMS_CONFIG_ATTRIBUTE;
    static const string REQUIRE_STRING_MATINHERIT;
    static const string REQUIRE_STRING_MATNODEGRAPH;
    static const string REQUIRE_STRING_OVERRIDE;

  private:
    class Cache;
    std::unique_ptr<Cache> _cache;
};

/// @class @ScopedUpdate
/// An RAII class for Document updates.
///
/// A ScopedUpdate instance calls Document::onBeginUpdate when created, and
/// Document::onEndUpdate when destroyed.
class ScopedUpdate
{
    public:
    ScopedUpdate(DocumentPtr doc) :
        _doc(doc)
    {
        _doc->onBeginUpdate();
    }
    ~ScopedUpdate()
    {
        _doc->onEndUpdate();
    }

    private:
    DocumentPtr _doc;
};

/// @class @ScopedDisableNotifications		
/// An RAII class for disabling all Document notifications.		
///		
/// A ScopedDisableNotifications instance calls Document::disableNotifications() when created, and		
/// Document::enableNotifications when destroyed.		
class ScopedDisableNotifications
{
  public:
    ScopedDisableNotifications(DocumentPtr doc) :
        _doc(doc)
    {
        _doc->disableNotifications();
    }
    ~ScopedDisableNotifications()
    {
        _doc->enableNotifications();
    }
  private:
    DocumentPtr _doc;
};

/// Create a new Document.
/// @relates Document
DocumentPtr createDocument();

} // namespace MaterialX

#endif
