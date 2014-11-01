#include "Config.h"

#include "Util.h"

#include <fstream>

Hook::Config *Hook::Config::sInst = nullptr;
const std::string Hook::Config::FILE_PATH = "settings.config";

const std::string FAV_VENDOR = "FAVDEV_VENDORID";
const std::string FAV_PROD = "FAVDEV_PRODUCTID";

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
	mFavProd()
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

	file.flush();

	mModified = false;

	return true;
}