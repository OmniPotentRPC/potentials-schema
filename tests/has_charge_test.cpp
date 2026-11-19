#include "Potentials.capnp.h"

#include <capnp/dynamic.h>
#include <capnp/message.h>
#include <capnp/schema-parser.h>
#include <capnp/schema.h>
#include <capnp/serialize.h>
#include <kj/filesystem.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

void require(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << "\n";
    std::exit(EXIT_FAILURE);
  }
}

template <typename StructType>
void checkSchemaOrdinalAndDefault(uint16_t expectedOrdinal) {
  auto schema = capnp::Schema::from<StructType>().asStruct();
  auto maybeField = schema.findFieldByName("hasCharge");
  require(maybeField != nullptr, "hasCharge field missing from struct schema");
  auto field = KJ_ASSERT_NONNULL(maybeField);
  auto ordinal = field.getProto().getOrdinal();
  require(ordinal.isExplicit(), "hasCharge ordinal must be explicit");
  require(
      ordinal.getExplicit() == expectedOrdinal,
      "hasCharge ordinal does not match expected value");
  auto slot = field.getProto().getSlot();
  require(
      slot.getType().isBool(),
      "hasCharge field type is not Bool");
  require(
      slot.getDefaultValue().isBool() && !slot.getDefaultValue().getBool(),
      "hasCharge default value is not false");
}

template <typename StructType>
void checkDefaultAndRoundTrip() {
  {
    capnp::MallocMessageBuilder builder;
    auto root = builder.initRoot<StructType>();
    require(!root.getHasCharge(), "builder default hasCharge must be false");

    auto words = capnp::messageToFlatArray(builder);
    capnp::FlatArrayMessageReader reader(words);
    auto decoded = reader.getRoot<StructType>();
    require(!decoded.getHasCharge(), "reader default hasCharge must be false");
  }

  {
    capnp::MallocMessageBuilder builder;
    auto root = builder.initRoot<StructType>();
    root.setCharge(0);
    root.setHasCharge(true);
    require(root.getHasCharge(), "builder hasCharge must read true after set");

    auto words = capnp::messageToFlatArray(builder);
    capnp::FlatArrayMessageReader reader(words);
    auto decoded = reader.getRoot<StructType>();
    require(decoded.getHasCharge(), "round trip hasCharge must read true");
    require(decoded.getCharge() == 0, "round trip charge must read 0");
  }

  {
    capnp::MallocMessageBuilder builder;
    auto root = builder.initRoot<StructType>();
    root.setCharge(-3);
    root.setHasCharge(true);
    root.setHasCharge(false);

    auto words = capnp::messageToFlatArray(builder);
    capnp::FlatArrayMessageReader reader(words);
    auto decoded = reader.getRoot<StructType>();
    require(
        !decoded.getHasCharge(),
        "round trip hasCharge must read false after reset");
    require(decoded.getCharge() == -3, "round trip charge must read -3");
  }
}

template <typename StructType>
void checkWireCompatibilityWithoutField(
    capnp::StructSchema legacySchema,
    int32_t sampleCharge) {
  {
    capnp::MallocMessageBuilder builder;
    auto root = builder.initRoot<StructType>();
    root.setCharge(sampleCharge);

    auto words = capnp::messageToFlatArray(builder);
    capnp::FlatArrayMessageReader reader(words);
    auto decoded = reader.getRoot<StructType>();
    require(
        !decoded.getHasCharge(),
        "message encoded without setting hasCharge must read false");
    require(
        decoded.getCharge() == sampleCharge,
        "message encoded without setting hasCharge must preserve charge");
  }

  {
    capnp::MallocMessageBuilder builder;
    builder.getRoot<capnp::AnyPointer>().initAsAnyStruct(0, 0);

    auto words = capnp::messageToFlatArray(builder);
    capnp::FlatArrayMessageReader reader(words);
    auto decoded = reader.getRoot<StructType>();
    require(
        !decoded.getHasCharge(),
        "empty wire struct predating hasCharge must read false");
  }

  {
    require(
        legacySchema.findFieldByName("hasCharge") == nullptr,
        "legacy schema must not define hasCharge");
    capnp::MallocMessageBuilder builder;
    auto legacyRoot = builder.initRoot<capnp::DynamicStruct>(legacySchema);
    legacyRoot.set("charge", sampleCharge);

    auto words = capnp::messageToFlatArray(builder);
    capnp::FlatArrayMessageReader reader(words);
    auto decoded = reader.getRoot<StructType>();
    require(
        !decoded.getHasCharge(),
        "message encoded with schema lacking hasCharge must read false");
    require(
        decoded.getCharge() == sampleCharge,
        "message encoded with schema lacking hasCharge must preserve charge");
  }
}

std::string stripTargetHasChargeFields(const std::string& schemaPath) {
  std::ifstream in(schemaPath);
  require(in.good(), "unable to open Potentials.capnp");
  std::ostringstream out;
  std::string line;
  int strippedCount = 0;
  while (std::getline(in, line)) {
    if (line.find("hasCharge @91 :Bool") != std::string::npos ||
        line.find("hasCharge @13 :Bool") != std::string::npos ||
        line.find("hasCharge @12 :Bool") != std::string::npos) {
      ++strippedCount;
      continue;
    }
    out << line << "\n";
  }
  require(strippedCount == 3, "expected all three hasCharge fields in schema");
  return out.str();
}

}  // namespace

int main() {
  checkSchemaOrdinalAndDefault<CPMDSystemSection>(91);
  checkSchemaOrdinalAndDefault<CPMDParams>(13);
  checkSchemaOrdinalAndDefault<CommonMethodSpec>(12);

  checkDefaultAndRoundTrip<CPMDSystemSection>();
  checkDefaultAndRoundTrip<CPMDParams>();
  checkDefaultAndRoundTrip<CommonMethodSpec>();

  {
    capnp::MallocMessageBuilder builder;
    auto config = builder.initRoot<PotentialConfig>();
    auto common = config.initCommon();
    common.setCharge(0);
    common.setHasCharge(true);

    auto cpmd = config.initCpmd();
    cpmd.setCharge(0);
    cpmd.setHasCharge(true);
    auto sections = cpmd.initInputSections(1);
    auto system = sections[0].initSystem();
    system.setCharge(0);
    system.setHasCharge(true);

    auto words = capnp::messageToFlatArray(builder);
    capnp::FlatArrayMessageReader reader(words);
    auto decoded = reader.getRoot<PotentialConfig>();
    require(
        decoded.getCommon().getHasCharge(),
        "PotentialConfig.common.hasCharge must round trip as true");
    require(
        decoded.getCpmd().getHasCharge(),
        "PotentialConfig.cpmd.hasCharge must round trip as true");
    require(
        decoded.getCpmd().getInputSections()[0].getSystem().getHasCharge(),
        "CPMDInputSection.system.hasCharge must round trip as true");
  }

  const std::string legacySource =
      stripTargetHasChargeFields(POTENTIALS_SCHEMA_PATH);
  auto memDir = kj::newInMemoryDirectory(kj::nullClock());
  {
    auto file = memDir->openFile(
        kj::Path({"Potentials.capnp"}),
        kj::WriteMode::CREATE | kj::WriteMode::MODIFY);
    file->writeAll(legacySource);
  }

  capnp::SchemaParser parser;
  auto parsed = parser.parseFromDirectory(
      *memDir, kj::Path({"Potentials.capnp"}), nullptr);

  checkWireCompatibilityWithoutField<CPMDSystemSection>(
      parsed.getNested("CPMDSystemSection").asStruct(), -1);
  checkWireCompatibilityWithoutField<CPMDParams>(
      parsed.getNested("CPMDParams").asStruct(), -2);
  checkWireCompatibilityWithoutField<CommonMethodSpec>(
      parsed.getNested("CommonMethodSpec").asStruct(), 2);

  return EXIT_SUCCESS;
}
