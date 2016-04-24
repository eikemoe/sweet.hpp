#pragma once

#include <vector>
#include <functional>
#include <string>
#include <ostream>
#include <type_traits>
#include <cstring>

enum class SweetqlFlags {
	NotPrimaryKey = 0,
	PrimaryKey = 1
};

enum class SweetqlTypes {
	Int,
	Float,
	String,
	Blob
};

inline std::ostream& operator<<(std::ostream& out, const SweetqlTypes& t) {
	switch(t) {
		case SweetqlTypes::Int: out<<"Int"; break;
		case SweetqlTypes::Float: out<<"Float"; break;
		case SweetqlTypes::String: out<<"String"; break;
		case SweetqlTypes::Blob: out<<"Blob"; break;
		default: out<<"Undefined"; break;
	}

	return out;
}

typedef void(*del)(void*);

template<class T>
using BlobGetterFunc = std::function<const void*(const T&)>;
template<class T>
using BlobSetterFunc = std::function<void(T&,const void*,size_t)>;
template<class T>
using SizeGetterFunc = std::function<size_t(const T&)>;
template<class T>
using StringGetterFunc = std::function<std::string(const T&)>;
template<class T>
using StringSetterFunc = std::function<void(T&,const char*)>;

template<typename T>
class SqlAttribute {
public:
	inline SqlAttribute(SweetqlTypes t) : type(t) {
		primaryKey = SweetqlFlags::NotPrimaryKey;
	}

	inline SqlAttribute(SweetqlTypes t, SweetqlFlags p) : type(t) {
		primaryKey = p;
	}

	inline virtual int64_t getInt(const T&) const { 
		throw std::logic_error("getInt not implemented"); 
	}
	inline virtual double getFloat(const T&) const { 
		throw std::logic_error("getFloat not implemented"); 
	}
	inline virtual std::string getString(const T&) const { 
		throw std::logic_error("getString not implemented"); 
	}
	inline virtual const void* getBlob(const T&) const { 
		throw std::logic_error("getBlob not implemented"); 
	}
	inline virtual size_t getBlobSize(const T&) const { 
		throw std::logic_error("getBlobSize not implemented"); 
	}
	inline virtual del getBlobDel(const T&) const { 
		throw std::logic_error("getBlobSize not implemented"); 
	}
	inline virtual void setInt(T&,int64_t) {
		throw std::logic_error("setInt not implemented"); 
	}
	inline virtual void setFloat(T&,double) {
		throw std::logic_error("setFloat not implemented"); 
	}
	//inline virtual void setString(T&,const std::string&) {
		//throw std::logic_error("setString not implemented"); 
	//}
	inline virtual void setString(T&,const char*) {
		throw std::logic_error("setString not implemented"); 
	}
	inline virtual void setBlob(T&,const void*,size_t) {
		throw std::logic_error("setBlob not implemented"); 
	}

	inline std::string getType() const {
		switch(this->type) {
		case SweetqlTypes::String: return "varchar";
		case SweetqlTypes::Int: return "integer";
		case SweetqlTypes::Float: return "real";
		case SweetqlTypes::Blob: return "blob";
		default: 
			throw std::logic_error(std::string("wroong type for getType"));
		}
	}

	SweetqlFlags primaryKey;
	SweetqlTypes type;
};

inline bool isPrimaryKey(SweetqlFlags flags) {
	return static_cast<int>(flags) & 
		static_cast<int>(SweetqlFlags::PrimaryKey);
}

template<typename T>
class SqlStringAttribute : public SqlAttribute<T> {
public:
	inline SqlStringAttribute(StringGetterFunc<T> sgf, StringSetterFunc<T> ssf) : 
		SqlAttribute<T>(SweetqlTypes::String), 
		sgf(sgf), ssf(ssf)
	{}

	inline SqlStringAttribute(StringGetterFunc<T> sgf, StringSetterFunc<T> ssf, 
			SweetqlFlags f) : 
		SqlAttribute<T>(SweetqlTypes::String, f), 
		sgf(sgf), ssf(ssf)
	{}

	inline std::string getString(const T& t) const { 
		return sgf(t);
	}
	inline void setString(T& t, const char* s) {
		ssf(t, s);
	}
private:
	StringGetterFunc<T> sgf;
	StringSetterFunc<T> ssf;
};

template<typename T, typename U>
class SqlIntAttribute : public SqlAttribute<T> {
public:
	inline SqlIntAttribute(U T::* i) : SqlAttribute<T>(SweetqlTypes::Int), integer(i) {}
	inline SqlIntAttribute(U T::* i, SweetqlFlags f) : 
		SqlAttribute<T>(SweetqlTypes::Int, f), integer(i) {}

	inline int64_t getInt(const T& t) const { 
		return t.*integer;
	}
	inline void setInt(T& t, int64_t i) {
		t.*integer = i;
	}
private:
	U T::* integer;
};

template<typename T, typename U>
class SqlFloatAttribute : public SqlAttribute<T> {
public:
	inline SqlFloatAttribute(U T::* i) : SqlAttribute<T>(SweetqlTypes::Float), fl(i) {}
	inline SqlFloatAttribute(U T::* i, SweetqlFlags f) : 
		SqlAttribute<T>(SweetqlTypes::Float, f), fl(i) {}

	inline double getFloat(const T& t) const { 
		return t.*fl;
	}
	inline void setFloat(T& t, double i) {
		t.*fl = i;
	}
private:
	U T::* fl;
};

template<typename T>
class SqlBlobAttribute : public SqlAttribute<T> {
public:
	inline SqlBlobAttribute(SizeGetterFunc<T> sgf,
			BlobGetterFunc<T> bgf, BlobSetterFunc<T> bsf) :
		SqlAttribute<T>(SweetqlTypes::Blob),
		sgf(sgf), bgf(bgf), bsf(bsf) {}

	inline virtual const void* getBlob(const T& t) const { 
		return this->bgf(t);
	}
	inline virtual size_t getBlobSize(const T& t) const { 
		return this->sgf(t);
	}
	inline virtual del getBlobDel(const T&) const { 
		return nullptr; // XXX use flags for this?
	}
	inline virtual void setBlob(T& t,const void* src,size_t count) {
		this->bsf(t, src, count);
	}
private:
	SizeGetterFunc<T> sgf; // function to get length of the blob
	BlobGetterFunc<T> bgf; // function to get the blob
	BlobSetterFunc<T> bsf; // function to set the blob
};

template<typename S, typename T>
std::shared_ptr<SqlAttribute<T>> makeAttr(S T::* i,
		typename std::enable_if<std::is_integral<S>::value, S>::type = 0) {
	return std::make_shared<SqlIntAttribute<T,S>>(i);
}

template<typename S, typename T>
std::shared_ptr<SqlAttribute<T>> makeAttr(S T::* i,
		typename std::enable_if<std::is_floating_point<S>::value, S>::type = 0) {
	return std::make_shared<SqlFloatAttribute<T,S>>(i);
}

template<typename S, typename T>
std::shared_ptr<SqlAttribute<T>> makeAttr(S T::* i,
		typename std::enable_if<std::is_same<S,std::string>::value, S>::type = "") {
	return std::make_shared<SqlStringAttribute<T>>(
			[i](const T& t){ return t.*i; },
			[i](T& t, const char* s){ t.*i = s; }
			);
}

template<typename T>
std::shared_ptr<SqlAttribute<T>> makeStringAttr( StringGetterFunc<T> sgf,
		StringSetterFunc<T> ssf) {
	return std::make_shared<SqlStringAttribute<T>>(sgf,ssf);
}

template<typename T>
std::shared_ptr<SqlAttribute<T>> makeBlobAttr(SizeGetterFunc<T> sgf,
		BlobGetterFunc<T> bgf, BlobSetterFunc<T> bsf) {
	return std::make_shared<SqlBlobAttribute<T>>( sgf, bgf, bsf );
}

template<typename S, typename T>
std::shared_ptr<SqlAttribute<T>> makeAttr(S T::* i,
		size_t /*size*/, del /*d*/) {
	return std::make_shared<SqlStringAttribute<T>>(i);
}

// With flags

template<typename S, typename T>
std::shared_ptr<SqlAttribute<T>> makeAttr(S T::* i, SweetqlFlags f,
		typename std::enable_if<std::is_integral<S>::value, S>::type = 0) {
	return std::make_shared<SqlIntAttribute<T,S>>(i, f);
}

template<typename S, typename T>
std::shared_ptr<SqlAttribute<T>> makeAttr(S T::* i, SweetqlFlags f,
		typename std::enable_if<std::is_floating_point<S>::value, S>::type = 0) {
	return std::make_shared<SqlFloatAttribute<T,S>>(i, f);
}

template<typename S, typename T>
std::shared_ptr<SqlAttribute<T>> makeAttr(S T::* i, SweetqlFlags f,
		typename std::enable_if<std::is_same<S,std::string>::value, S>::type = "") {
	return std::make_shared<SqlStringAttribute<T>>(
			[i](const T& t){ return t.*i; },
			[i](T& t, const char* s){ t.*i = s; },
			f);
}

template<typename T>
std::shared_ptr<SqlAttribute<T>> makeStringAttr(StringGetterFunc<T> sgf, 
		StringSetterFunc<T> ssf, SweetqlFlags f) {
	return std::make_shared<SqlStringAttribute<T>>(sgf, ssf, f);
}

template<typename S, typename T>
std::shared_ptr<SqlAttribute<T>> makeAttr(S T::* i, SweetqlFlags f,
		size_t /*size*/, del /*d*/) {
	return std::make_shared<SqlStringAttribute<T>>(i, f);
}

template<typename T>
class SqlColumn {
public:
	inline SqlColumn(const std::string& a, std::shared_ptr<SqlAttribute<T>> at) : 
			attrName(a), attr(at) {
	}

	std::string attrName;
	std::shared_ptr<SqlAttribute<T>> attr;
};

template<typename T>
class SqlTable {
public:
	template<typename S>
	static void storeColumn(SqlTable& t, S col) {
		t.column.push_back(col);
	}

	template<typename S, typename... Col>
	static void storeColumn(SqlTable& t, S col, Col... c) {
		t.column.push_back(col);
		storeColumn(t,c...);
	}

	template<typename... Col>
	static SqlTable<T> sqlTable(const std::string& n, Col... cs) {
		SqlTable<T> tab;
		tab.name = n;
		storeColumn(tab, cs...);
		return tab;
	}

	std::string name;
	std::vector<SqlColumn<T>> column;
};
