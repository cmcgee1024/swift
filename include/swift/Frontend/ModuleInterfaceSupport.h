//===------ ModuleInterfaceSupport.h - swiftinterface files -----*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2019 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_FRONTEND_MODULEINTERFACESUPPORT_H
#define SWIFT_FRONTEND_MODULEINTERFACESUPPORT_H

#include "swift/AST/PrintOptions.h"
#include "swift/Basic/LLVM.h"
#include "swift/Basic/Version.h"
#include "llvm/Support/Regex.h"

#define SWIFT_INTERFACE_FORMAT_VERSION_KEY "swift-interface-format-version"
#define SWIFT_COMPILER_VERSION_KEY "swift-compiler-version"
#define SWIFT_MODULE_FLAGS_KEY "swift-module-flags"
#define SWIFT_MODULE_FLAGS_IGNORABLE_KEY "swift-module-flags-ignorable"
#define SWIFT_MODULE_FLAGS_IGNORABLE_PRIVATE_KEY                               \
  "swift-module-flags-ignorable-private"

namespace swift {

class ASTContext;
class ModuleDecl;

/// Options for controlling the generation of the .swiftinterface output.
struct ModuleInterfaceOptions {
  /// Should we prefer printing TypeReprs when writing out types in a module
  /// interface, or should we fully-qualify them?
  bool PreserveTypesAsWritten = false;

  /// Use aliases when printing references to modules to avoid ambiguities
  /// with types sharing a name with a module.
  bool AliasModuleNames = false;

  /// See \ref FrontendOptions.PrintFullConvention.
  /// [TODO: Clang-type-plumbing] This check should go away.
  bool PrintFullConvention = false;

  /// Print some ABI details for public symbols as comments that can be
  /// parsed by another tool.
  bool ABIComments = false;

  struct InterfaceFlags {
    /// Copy of all the command-line flags passed at .swiftinterface
    /// generation time, re-applied to CompilerInvocation when reading
    /// back .swiftinterface and reconstructing .swiftmodule.
    std::string Flags = "";

    /// Flags that should be emitted to the .swiftinterface file but are OK to
    /// be ignored by the earlier version of the compiler.
    std::string IgnorableFlags = "";
  };

  /// Flags which appear in all .swiftinterface files.
  InterfaceFlags PublicFlags = {};

  /// Flags which appear in both the private and package .swiftinterface files,
  /// but not the public interface.
  InterfaceFlags PrivateFlags = {};

  /// Flags which appear only in the .package.swiftinterface.
  InterfaceFlags PackageFlags = {};

  /// Print imports that are missing from the source and used in API.
  bool PrintMissingImports = true;

  /// Intentionally print invalid syntax into the file.
  bool DebugPrintInvalidSyntax = false;

  /// A list of modules we shouldn't import in the public interfaces.
  std::vector<std::string> ModulesToSkipInPublicInterface;

  /// A mode which decides whether the printed interface contains package, SPIs, or public/inlinable declarations.
  PrintOptions::InterfaceMode InterfaceContentMode = PrintOptions::InterfaceMode::Public;
  bool printPublicInterface() const {
    return InterfaceContentMode == PrintOptions::InterfaceMode::Public;
  }
  bool printPackageInterface() const {
    return InterfaceContentMode == PrintOptions::InterfaceMode::Package;
  }
  void setInterfaceMode(PrintOptions::InterfaceMode mode) {
    InterfaceContentMode = mode;
  }
};

extern version::Version InterfaceFormatVersion;
std::string getSwiftInterfaceCompilerVersionForCurrentCompiler(ASTContext &ctx);

/// A regex that matches lines like this:
///
///     // swift-interface-format-version: 1.0
///
/// and extracts "1.0".
llvm::Regex getSwiftInterfaceFormatVersionRegex();

/// A regex that matches lines like this:
///
///     // swift-compiler-version: Apple Swift version 5.8 (swiftlang-5.8.0.117.59)
///
/// and extracts "Apple Swift version 5.8 (swiftlang-5.8.0.117.59)".
llvm::Regex getSwiftInterfaceCompilerVersionRegex();

/// A regex that matches strings like this:
///
///     Apple Swift version 5.8
///
/// and extracts "5.8".
llvm::Regex getSwiftInterfaceCompilerToolsVersionRegex();

/// Emit a stable module interface for \p M, which can be used by a client
/// source file to import this module, subject to options given by \p Opts.
///
/// Unlike a serialized module, the textual format generated by
/// emitSwiftInterface is intended to be stable across compiler versions while
/// still describing the full ABI of the module in question.
///
/// The initial plan for this format can be found at
/// https://forums.swift.org/t/plan-for-module-stability/14551/
///
/// \return true if an error occurred
///
/// \sa swift::serialize
bool emitSwiftInterface(raw_ostream &out,
                        ModuleInterfaceOptions const &Opts,
                        ModuleDecl *M);

} // end namespace swift

#endif
