#ifndef __CTRLHOOK_CONFIG_H_
#define __CTRLHOOK_CONFIG_H_

#include <string>
#include <forward_list>

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

		bool hasProcessList() const;
		const std::forward_list<std::wstring> &getProcessList() const;

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