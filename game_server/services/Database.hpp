/// @author    Chnossos
/// @date      Created on 2026-05-05

#pragma once

// Third-party includes
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

class Database
{
public:
    static void init();
    static auto instance() -> SQLite::Database &;
};
