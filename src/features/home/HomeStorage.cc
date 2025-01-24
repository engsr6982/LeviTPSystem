#include "HomeStorage.h"
#include "common/Global.h"
#include "common/Utils.h"
#include "fmt/core.h"


namespace tps {

HomeStorage::Data::Data(string name, int x, int y, int z, int dimid)
: x(x),
  y(y),
  z(z),
  dimid(dimid),
  name(std::move(name)),
  createTime(""),
  updateTime("") {}

HomeStorage::Data::operator string() const { return fmt::format("{0}({1},{2},{3})", dimid2str(dimid), x, y, z); }


// HomeStorage
void HomeStorage::load() {};
void HomeStorage::save() {};

std::string HomeStorage::getPrefix() { return "home_"; };


HomeStorage& HomeStorage::getInstance() {
    static HomeStorage instance;
    return instance;
}

HomeStorage::DataPtr HomeStorage::load(std::string_view key) { return nullptr; };
void                 HomeStorage::save(std::string_view key, HomeStorage::DataPtr data) {};


HomeStorage::Data* HomeStorage::getHome(string const& name) { return nullptr; }

void HomeStorage::addHome(HomeStorage::DataPtr data) {}

void HomeStorage::deleteHome(string const& name) {}

void HomeStorage::updateHome(string const& name, HomeStorage::Data* data) {}


} // namespace tps