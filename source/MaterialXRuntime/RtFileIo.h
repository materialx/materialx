//
// TM & (c) 2019 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef MATERIALX_RTFILEIO_H
#define MATERIALX_RTFILEIO_H

#include <MaterialXRuntime/Library.h>
#include <MaterialXRuntime/RtStage.h>

#include <MaterialXCore/Document.h>
#include <MaterialXFormat/File.h>
#include <MaterialXFormat/XmlIo.h>

/// @file
/// TODO: Docs

namespace MaterialX
{

/// @class RtReadOptions
/// A set of options for controlling the behavior of read functions.
class RtReadOptions
{
  public:
    using ReadFilter = std::function<bool(const ElementPtr& elem)>;

  public:
    RtReadOptions() :
        skipConflictingElements(true),
        readFilter(nullptr)
    {
    }
    ~RtReadOptions() { }

    /// If true, duplicate elements with non-identical content will be skipped;
    /// otherwise they will trigger an exception.  Defaults to false.
    bool skipConflictingElements;

    /// Filter function type used for filtering elements during read.
    /// If the filter returns false the element will not be read.
    ReadFilter readFilter;
};
    
/// @class RtWriteOptions
/// A set of options for controlling the behavior of write functions.
class RtWriteOptions
{
  public:
    using WriteFilter = std::function<bool(const RtObject& obj)>;

  public:
     RtWriteOptions() :
          writeIncludes(true),
          writeFilter(nullptr),
          materialWriteOp(NONE)
    {
    }
    ~RtWriteOptions() { }

    /// If true, elements with source file markings will be written as
    /// includes rather than explicit data.  Defaults to true.
    bool writeIncludes;

    /// Filter function type used for filtering objects during write.
    /// If the filter returns false the object will not be written.
    WriteFilter writeFilter;

    /// Enum that specifies how to generate material elements.
    ///
    /// NONE: don't generate material elements
    ///
    /// WRITE: generate material elements from surface shaders
    ///
    /// DELETE: delete source surface shaders (must be used with
    /// WRITE)
    ///
    /// LOOK: generate a look for the material element (must be
    /// used with WRITE)
    ///
    /// WRITE_DELETE: generate material elements from surface
    /// shaders and delete the surface shaders
    ///
    /// WRITE_LOOKS: generate material elements for surface shaders
    /// and write out looks
    ///
    /// WRITE_LOOKS_DELETE: generate material elements from
    /// surface shaders, delete the surface shaders and write out
    /// looks
    ///
    /// TODO: Look into removing this once Material nodes are supported
    enum MaterialWriteOp{ NONE               = 0,
                          WRITE              = 1 << 0,
                          DELETE             = 1 << 1,
                          LOOK               = 1 << 2,
                          WRITE_DELETE       = WRITE | DELETE,
                          WRITE_LOOKS        = WRITE | LOOK,
                          WRITE_LOOKS_DELETE = WRITE | LOOK | DELETE };

    MaterialWriteOp materialWriteOp;
};

/// API for read and write of data from MaterialX files
/// to runtime stages.
class RtFileIo
{
public:
    /// Constructor attaching this API to a stage.
    RtFileIo(RtStagePtr stage) :
        _stage(stage)
    {
    }

    /// Attach this API instance to a new stage.
    void setStage(RtStagePtr stage)
    {
        _stage = stage;
    }

    /// Read contents from a stream
    /// If a filter is used only elements accepted by the filter
    /// will be red from the document.
    void read(std::istream& stream, const RtReadOptions* options = nullptr);

    /// Write all stage contents to stream.
    /// If a filter is used only elements accepted by the filter
    /// will be written to the document.
    void write(std::ostream& stream, const RtWriteOptions* writeOptions = nullptr);

    /// Read contents from a file path.
    /// If a filter is used only elements accepted by the filter
    /// will be read from the document.
    void read(const FilePath& documentPath, const FileSearchPath& searchPaths, const RtReadOptions* options = nullptr);

    /// Write all stage contents to a document.
    /// If a filter is used only elements accepted by the filter
    /// will be written to the document.
    void write(const FilePath& documentPath, const RtWriteOptions* writeOptions = nullptr);

    /// Read all contents from one or more libraries.
    /// All MaterialX files found inside the given libraries will be read.
    void readLibraries(const StringVec& libraryPaths, const FileSearchPath& searchPaths);

private:
    RtStagePtr _stage;
};

}

#endif
