#pragma once
#include <mutex>
#include <chrono>
#include <filesystem>
#include "../sys.h"
#include "doc_text_model.h"
namespace doc
{
    namespace repo
    {
        using path = std::filesystem::path;
        using time = std::filesystem::file_time_type;

        struct data
        {
            path path;
            time filetime;
            time edittime;
            text saved_text;
            text_model model;
            std::shared_mutex mutex;

            data (std::filesystem::path path) : path(path) {}

            expected<nothing> load () try
            {
                std::unique_lock guard{mutex};

                time last_write_time = std::filesystem::last_write_time(path);
                if (edittime != time{} and edittime >= last_write_time or
                    edittime != time{} and edittime < last_write_time and
                    sys::dialog("AE proto-studio", "File" "\n" + path.string() +
                    "\n" "has been modified outside of the editor."
                    "\n" "Do you want to reload it?",
                    sys::choice::yes_no) == "no")
                    return nothing{};

                str s;
                {
                    std::ifstream stream(path); s = std::string{(
                    std::istreambuf_iterator<char>(stream)),
                    std::istreambuf_iterator<char>()};
                }
                if (s.starts_with("\xEF" "\xBB" "\xBF"))
                    s.upto(3).erase(); // UTF-8 BOM

                model = text_model{s};
                filetime = edittime = last_write_time;
                saved_text = model;
                return nothing{};
            }
            catch (const std::exception & e) {
                return error(path.string() +
                    ": " + e.what());
            }

            expected<nothing> save () try
            {
                std::shared_lock guard{mutex};

                if (edittime == filetime) return nothing{};

                if (!std::filesystem::exists(path) and
                    sys::dialog("AE proto-studio", "File" "\n" + path.string() +
                    "\n" "has been deleted." "\n" "Do you want to create it?",
                    sys::choice::yes_no) == "no")
                    return nothing{};

                time write_time = std::filesystem::last_write_time(path);
                if (edittime != time{} and edittime < write_time and
                    sys::dialog("AE proto-studio", "File" "\n" + path.string() +
                    "\n" "has been modified outside of the editor."
                    "\n" "Do you want to rewrite it?",
                    sys::choice::yes_no) == "no")
                    return nothing{};

                std::filesystem::path temp = path;
                temp.replace_extension(temp.extension().string() + "~");
                {
                    std::ofstream stream(temp);
                    for (auto & line : model.lines)
                        stream << str(line, "") << "\n";
                }
                std::filesystem::rename(temp, path);
                filetime = edittime = std::filesystem::last_write_time(path);
                saved_text = model;
                return nothing{};
            }
            catch (const std::exception & e) {
                return error(path.string() +
                    ": " + e.what());
            }
        };

        inline std::map<path, data> map; std::shared_mutex mutex;

        expected<nothing> load (path path, text_model & model, time & time)
        {
            if (path == std::filesystem::path{})
                return nothing{};

            decltype(map)::iterator it;
            {
                std::unique_lock guard{mutex};
                it = map.try_emplace(path, path).first;
            }
            auto rc = it->second.load(); if (!rc.ok()) return rc;
            std::shared_lock data_guard{it->second.mutex};
            model = it->second.model;
            time = it->second.edittime;
            return nothing{};
        }

        expected<nothing> load (path path, text_model & model)
        {
            time time; return load(path, model, time);
        }

        expected<nothing> edittime (path path, time & time)
        {
            if (path == std::filesystem::path{})
                return nothing{};

            decltype(map)::iterator it;
            {
                std::unique_lock guard{mutex};
                it = map.try_emplace(path, path).first;
            }
            auto rc = it->second.load(); if (!rc.ok()) return rc;
            std::shared_lock data_guard{it->second.mutex};
            time = it->second.edittime;
            return nothing{};
        }

        expected<nothing> modify (path path, const text_model & model)
        {
            decltype(map)::iterator it;
            {
                std::shared_lock guard{mutex};
                it = map.find(path);
                if (it == map.end())
                    return error(path.string() +
                        ": file doesn't open");
            }
            std::unique_lock data_guard{it->second.mutex};
            it->second.model = model; // problem: full text copy and comparison
            it->second.edittime = it->second.saved_text.lines == model.lines ?
            it->second.filetime : time::clock::now();
            return nothing{};
        }

        expected<nothing> save (path path)
        {
            decltype(map)::iterator it;
            {
                std::shared_lock guard{mutex};
                it = map.find(path);
                if (it == map.end())
                    return error(path.string() +
                        ": file doesn't open");
            }
            return it->second.save();
        }

        expected<nothing> save_all ()
        {
            std::shared_lock guard{mutex};

            for (auto & [path, data] : map) {
                auto rc = data.save();
                if (!rc.ok())
                    return rc;
            }
            return nothing{};
        }

        expected<nothing> reload ()
        {
            std::shared_lock guard{mutex};

            for (auto & [path, data] : map) {
                auto rc = data.load();
                if (!rc.ok())
                    return rc;
            }
            return nothing{};
        }

        bool saveable (path path)
        {
            std::shared_lock guard{mutex};

            auto it = map.find(path);
            if (it == map.end()) return false;
            auto & data = it->second;
            std::shared_lock data_guard{data.mutex};
            return data.edittime > data.filetime;
        }

        bool saveable ()
        {
            std::shared_lock guard{mutex};

            for (auto & [path, data] : map) {
                std::shared_lock data_guard{data.mutex};
                if (data.edittime > data.filetime)
                    return true;
            }

            return false;
        }

        void close ()
        {
            std::shared_lock guard{mutex};

            for (auto & [path, data] : map)
            {
                std::shared_lock data_guard{data.mutex};
                if (data.edittime > data.filetime and
                    sys::dialog("AE proto-studio", "File" "\n" + path.string() +
                    "\n" "has been modified."
                    "\n" "Do you want to save it?",
                    sys::choice::yes_no) == "yes")
                    data.save();
            }
        }
    }
}
