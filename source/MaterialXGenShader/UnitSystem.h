//
// TM & (c) 2017 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef MATERIALX_UNIT_SYSTEM_H
#define MATERIALX_UNIT_SYSTEM_H

/// @file
/// Unit system classes

#include <MaterialXGenShader/Library.h>

#include <MaterialXGenShader/UnitConverter.h>

#include <MaterialXGenShader/ShaderNode.h>
#include <MaterialXGenShader/ShaderNodeImpl.h>
#include <MaterialXGenShader/TypeDesc.h>

#include <MaterialXCore/Document.h>

#include <array>

namespace MaterialX
{

class ShaderGenerator;

/// A shared pointer to a UnitSystem
using UnitSystemPtr = shared_ptr<class UnitSystem>;

/// @struct @UnitTransform
/// Structure that represents unit transform information
struct UnitTransform
{
    UnitTransform(const string& ss, const string& ts, const TypeDesc* t, const string& unittype);

    string sourceUnit;
    string targetUnit;
    const TypeDesc* type;
    string unitType;

    /// Comparison operator
    bool operator==(const UnitTransform &other) const
    {
        return sourceUnit == other.sourceUnit &&
            targetUnit == other.targetUnit &&
            type == other.type &&
            unitType == other.unitType;
    }
};

/// @class UnitSystem
/// Abstract base class for a unit system
class UnitSystem
{
  public:
    virtual ~UnitSystem() { }

    /// Create a new UnitSystem
    static UnitSystemPtr create(const string& language);

    /// Return the UnitSystem name
    virtual const string& getName() const
    {
        return UnitSystem::UNITSYTEM_NAME;
    }

    /// Assign unit converter registry replacing any previous assignment
    virtual void setUnitConverterRegistry(UnitConverterRegistryPtr registry);

    /// Returns the currently assigned unit converter registry
    virtual UnitConverterRegistryPtr getUnitConverterRegistry() const;

    /// assign document with unit implementations replacing any previously loaded content.
    virtual void loadLibrary(DocumentPtr document);

    /// Returns whether this unit system supports a provided transform
    bool supportsTransform(const UnitTransform& transform) const;

    /// Create a node to use to perform the given unit space transformation.
    ShaderNodePtr createNode(ShaderGraph* parent, const UnitTransform& transform, const string& name,
        GenContext& context) const;

    /// Returns an implementation name for a given transform
    virtual string getImplementationName(const UnitTransform& transform, const string& unitname) const;

    static const string UNITSYTEM_NAME;
    static const string DISTANCE_UNIT_TARGET_NAME;

  protected:
    // Protected constructor
    UnitSystem(const string& language);

  protected:
    UnitConverterRegistryPtr _unitRegistry;
    DocumentPtr _document;
    string _language;
};

} // namespace MaterialX

#endif
