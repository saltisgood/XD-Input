#ifndef __CTRLHOOK_CONFIG_H_
#define __CTRLHOOK_CONFIG_H_

#include <string>
#include <forward_list>

namespace Hook
{
	// Class that handles the config of the program between runs. Singleton design.
	// .create() must be called before use. .destroy() can be called to release
	// the memory (not that it's a big deal).
	class Config
	{
	public:
		// The path to the config file (relative to the working directory).
		static const std::string FILE_PATH;

		// Attempt to create the Config instance and read from file. Returns true if the file
		// exists, false otherwise.
		static bool create();
		// Destroy the Config instance if it is in use.
		static void destroy();

		// Get the Config instance if it's been created.
		static Config *get();

	private:
		Config();
		~Config();

		bool open(const std::string &path);

	public:
		// Save the Config instance to file. Returns true if the file was written successfully,
		// false otherwise.
		bool save() const;

		// Check whether the user has selected a preferred device to connect to if it's available.
		bool hasFavouriteDevice() const;
		// Get the Vendor ID of the preferred device. Only a valid call if .hasFavouriteDevice() returns true.
		unsigned short getFavouriteVendorID() const;
		// Get the Product ID of the preferred device. Only a valid call if .hasFavourteDevice() returns true.
		unsigned short getFavouriteProductID() const;

		// Set the Vendor ID of the preferred device to be saved for later.
		void setFavouriteVendorID(unsigned short vendorId);
		// Set the Product ID of the preferred device to be saved for later.
		void setFavouriteProductID(unsigned short productId);

		// Check whether the user has selected a list of processes to use the program for.
		bool hasProcessList() const;
		// Get the list of processes to user the program for. If .hasProcessList() returns false, the list is empty.
		const std::forward_list<std::wstring> &getProcessList() const;

		// Check whether the program should quit upon the process that it was being used with ends. Only valid if
		// .hasProcessList() returns true.
		bool shouldEndOnGameProcDeath() const;

	private:
		static Config *sInst;

		mutable bool mModified;

		bool mHasFavDevice;
		unsigned short mFavVendor;
		unsigned short mFavProd;

		std::forward_list<std::wstring> mProcList;

		bool mEndWithGameProc;
	};

#pragma region inlines

	inline void Config::destroy()
	{
		if (sInst)
		{
			delete sInst;
			sInst = nullptr;
		}
	}

	inline Config *Config::get()
	{
		return sInst;
	}

	inline bool Config::hasFavouriteDevice() const
	{
		return mHasFavDevice;
	}

	inline unsigned short Config::getFavouriteVendorID() const
	{
		return mFavVendor;
	}

	inline unsigned short Config::getFavouriteProductID() const
	{
		return mFavProd;
	}

	inline void Config::setFavouriteVendorID(unsigned short vend)
	{
		mFavVendor = vend;
		mHasFavDevice = true;
		mModified = true;
	}

	inline void Config::setFavouriteProductID(unsigned short prod)
	{
		mFavProd = prod;
		mHasFavDevice = true;
		mModified = true;
	}

	inline bool Config::hasProcessList() const
	{
		return !mProcList.empty();
	}

	inline const std::forward_list<std::wstring> &Config::getProcessList() const
	{
		return mProcList;
	}

	inline bool Config::shouldEndOnGameProcDeath() const
	{
		return mEndWithGameProc;
	}

#pragma endregion
}

#endif