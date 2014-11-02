#include "Config.h"

#include "Util.h"

#include <fstream>

Hook::Config *Hook::Config::sInst = nullptr;
const std::string Hook::Config::FILE_PATH = "settings.config";

const char FAV_VENDOR[] = "FAVDEV_VENDORID";
const char FAV_PROD[] = "FAVDEV_PRODUCTID";
const char PROC_LIST[] = "PROCESS_LIST";
const char END_WITH_GAME[] = "QUIT_ON_GAME_CLOSE";

const char TRUE_STR[] = "TRUE";
const char FALSE_STR[] = "FALSE";

bool Hook::Config::create()
{
	if (!sInst)
	{
		if (!(sInst = new Config())->open(FILE_PATH))
		{
			sInst->save();
			return false;
		}

		return true;
	}

	return true;
}

Hook::Config::Config() :
	mModified(false),
	mHasFavDevice(false),
	mFavVendor(),
	mFavProd(),
	mProcList(),
	mEndWithGameProc(false)
{}

Hook::Config::~Config()
{
	if (mModified)
	{
		Hook::Config::save();
	}
}

bool Hook::Config::open(const std::string &path)
{
	std::ifstream file(path);

	if (!file)
	{
		return false;
	}

	char buff[256];

	file.getline(buff, sizeof(buff));

	while (file)
	{
		std::string line(buff);

		if (!line.empty())
		{
			auto pos(line.find_first_of('='));

			if (pos != std::string::npos)
			{
				auto key(line.substr(0, pos));
				auto val(line.substr(pos + 1, std::string::npos));

				try {
					if (!key.compare(FAV_VENDOR))
					{
						mFavVendor = static_cast<unsigned short>(std::stoul(val));
					}
					else if (!key.compare(FAV_PROD))
					{
						mFavProd = static_cast<unsigned short>(std::stoul(val));
					}
					else if (!key.compare(PROC_LIST))
					{
						while (!val.empty() && (pos = val.find_first_of(';')) != std::string::npos)
						{
							key = val.substr(0, pos);
							val = val.substr(pos + 1, std::string::npos);

							mProcList.insert_after(mProcList.before_begin(), byteToWideString(key));
						}
					}
					else if (!key.compare(END_WITH_GAME))
					{
						mEndWithGameProc = !val.compare(TRUE_STR);
					}
					else
					{
						static const std::string errorMsg("Unrecognised line: ");
						errorMessage(errorMsg + line);
					}
				}
				catch (const std::invalid_argument &exc)
				{
					static const std::string errorMsg("Error during config parsing: ");
					errorMessage(errorMsg + exc.what());
				}
			}
		}

		file.getline(buff, sizeof(buff));
	}

	if (mFavProd != 0 || mFavVendor != 0)
	{
		mHasFavDevice = true;
	}

	return true;
}

bool Hook::Config::save() const
{
	std::ofstream file(FILE_PATH);

	if (!file)
	{
		return false;
	}

	file << FAV_VENDOR << "=" << mFavVendor << "\n"
		<< FAV_PROD << "=" << mFavProd << std::endl;

	if (hasProcessList())
	{
		file << PROC_LIST << "=";

		for (auto &val : mProcList)
		{
			file << wideToByteString(val) << ';';
		}

		file << std::endl;
	}
	else
	{
		file << PROC_LIST << "=ExampleProcess1.exe;ExampleProcess2.exe;" << std::endl;
	}

	file << END_WITH_GAME << "=" << (mEndWithGameProc ? TRUE_STR : FALSE_STR) << std::endl;

	file.flush();

	mModified = false;

	return true;
}