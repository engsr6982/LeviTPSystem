#include "IStorage.h"
#include "core/database/Storage.h"

namespace tps {


ll::data::KeyValueDB* IStorage::getDB() { return Storage::getInstance().getDB(); }

bool IStorage::isConnected() { return Storage::getInstance().isConnected(); }

void IStorage::connect() { Storage::getInstance().connect(); }

void IStorage::disconnect() { Storage::getInstance().disconnect(); }

void IStorage::initialize() { Storage::getInstance().initialize(); }


} // namespace tps