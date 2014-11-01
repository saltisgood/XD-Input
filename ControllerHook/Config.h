#ifndef __CTRLHOOK_CONFIG_H_
#define __CTRLHOOK_CONFIG_H_

#include <string>

namespace Hook
{
	class Config
	{
	public:
		static const std::string FILE_PATH;

		static bool create();
		static void destroy();

		static Config *get();

	private:
		Config();
		~Config();

		bool open(const std::string &path);

	public:
		bool save() const;

		bool hasFavouriteDevice() const;
		unsigned short getFavouriteVendorID() const;
		unsigned short getFavouriteProductID() const;

		void setFavouriteVendorID(unsigned short vendorId);
		void setFavouriteProductID(unsigned short productId);

	private:
		static Config *sInst;

		mutable bool mModified;

		bool mHasFavDevice;
		unsigned short mFavVendor;
		unsigned short mFavProd;
	};

#pragma region inlines

	inline void Config::destroy()
	{
		if (Config::sInst)
		{
			delete Config::sInst;
			Config::sInst = nullptr;
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

#pragma endregion
}

#endif