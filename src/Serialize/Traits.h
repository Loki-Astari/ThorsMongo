#ifndef THORS_ANVIL_SERIALIZE_TRAITS_H
#define THORS_ANVIL_SERIALIZE_TRAITS_H
/*
 *  This file is all about defineing the template type Traits<T>
 *
 *  Two macros are provides to facilitate the building of Traits<T>
 *  specializations for user defined types.
 *
 *      ThorsAnvil_MakeTrait(DataType, ...)
 *      ThorsAnvil_ExpandTrait(ParentType, DataType, ...)
 *      ThorsAnvil_Template_MakeTrait(TemplateParameterCount, DataType, ...)
 *      ThorsAnvil_Template_ExpandTrait(TemplateParameterCount, ParentType, DataType, ...)
 *
 *      ThorsAnvil_PointerAllocator(DataType, Action)
 *      ThorsAnvil_MakeEnum(EnumType, EnumValues...)
 *
 *      ThorsAnvil_PolyMorphicSerializer(Type)
 *      ThorsAnvil_RegisterPolyMorphicType(Type)
 *
 *      ThorsAnvil_MakeTraitCustomSerialize(Type, SerializableType)
 *
 *      ThorsAnvil_MakeOverride(DataType, { <From>, <To>}... )
 *      ThorsAnvil_Template_MakeOverride(Count, DataType, { <From>, <To>}... )
 *
 *      ThorsAnvil_MakeFilter(DataType, filter)
 *      ThorsAnvil_Template_MakeFilter(Count, DataType, filter)
 *
 * --------------------------------------------------------------------------
 *
 *      [[Depricated]]
 *      Please don't use. Maintained for backward compatibility.
 *      ThorsAnvil_MakeTraitCustom(DataType)    // Will use operator << and operator >>
 *
 * --------------------------------------------------------------------------
 *
 *      Most commonly used:
 *      These are used for normal structures that you want to serialize.
 *              ThorsAnvil_MakeTrait(Type, Member-1, Member-2 etc)
 *
 *              ThorsAnvil_ExpandTrait(Parent-Type, Type, Member-1, Member-2 etc)
 *
 *      If your types are template types we need to know the number of template parameters.
 *      So you can use these alternatives for templated classes:
 *
 *              ThorsAnvil_Template_MakeTrait(Template-Param-Count, Type, Member-1, Member-2 etc)
 *              ThorsAnvil_Template_ExpandTrait(Template-Param-Count, Parent-Type, Type, Member-1, Member-2 etc)
 *
 *
 *      If you need special processing for pointers then you can use the following:
 *      Note if your object simply used new/delete and can create a default object then you don't need
 *      to use this. This is only required if you need some special creation/deletion processes.
 *
 *              ThorsAnvil_PointerAllocator(Type, AllocationType)
 *
 *              AllocationType::alloc()             // Called to default allocate an object. default new
 *              AllocationType::release()           // Called to release object.             default delete
 *
 *      Enum values are treated like strings when serialized:
 *
 *              ThorsAnvil_MakeEnum(Enum-Type, Enum-1, Enum-2 etc)
 *
 *      If you want pointers to handle polymorphic pointers
 *      Then we need some extra information:
 *
 *              Add the following to your class definition:
 *                  ThorsAnvil_PolyMorphicSerializer(Type)
 *
 *              Then in a source file add the following line:
 *                  ThorsAnvil_RegisterPolyMorphicType(Type)
 *
 *      The new version of Custom serializable:
 *
 *              ThorsAnvil_MakeTraitCustomSerialize(Type, SerializableType)
 *
 *      To get this to work you need to define the type SerializableType:
 *
 *              SerializableType::write(Printer, object)
 *              SerializableType::read(Parser, object)
 *
 *
 *      "object" is the object you want to serialize.
 *      Printer/Parser will be derived from
 *          PrinterInterface
 *          ParserInterface
 *
 *      You can write versions that use these generic types or you can
 *      have implementations for specific version.
 *          eg. BsonParser/BsonPrinter
 *
 *      The code will link with the most appropriate version:
 *
 * Sometimes you can not use the key names you want (they are reserved words or contain
 * symbols that are not valid in identifiers in C++). So we provide a way to override
 * the member names supplied in the ThorsAnvil_MakeTrait() so they will be mapped to
 * the appropriate key value in the JSON/BSON/YAML file.
 *
 *      ThorsAnvil_MakeOverride(DataType, { <From>, <To>}... )
 *      ThorsAnvil_Template_MakeOverride(Count, DataType, { <From>, <To>}... )
 *
 * Normally you would use ThorsAnvil_MakeOverride but if "DataType" is a template
 * type then you will need to use the ThorsAnvil_Template_MakeOverride and specify
 * the number of template parameters in "Count".
 *
 *          DataType:               The type we are overrideing
 *              { <From>, <To>}:    A set of mappings.
 *                                      From: The C++ identifier.
 *                                      To:   The Key used in the JSON/BSON/YAML file.
 *
 * --------------------------------------------------------------------------
 *
 *      Examples:
 *
 *      Bank.h
 *          #include "Traits.h"
 *          #include "Serialize.h"
 *          #include "SerUtil.h"
 *
 *          #include "JsonThor.h"
 *          #include "YamlThor.h"
 *          #include "BsonThor.h"
 *
 *
 *          #include "test/SerializeTest.h"
 *          #include <vector>
 *
 *
 *          namespace OnLineBank
 *          {
 *              enum TransType {Error, Deposit, Withdraw, Correction};
 *              struct ID
 *              {
 *                  long id;
 *                  ID() : id(-1){}
 *                  ID(long id): id(id){}
 *              };
 *              struct SerializeID: public ThorsAnvil::Serialize::DefaultCustomSerializer<OnLineBank::ID>
 *              {
 *                  virtual void writeJson(ThorsAnvil::Serialize::JsonPrinter& printer, ID const& object)   override
 *                  {
 *                      printer.stream() << object.id;
 *                  }
 *                  virtual void readJson(ThorsAnvil::Serialize::JsonParser& parser, ID& object)            override
 *                  {
 *                      parser.stream() >> object.id;
 *                  }
 *
 *                  //virtual void writeYaml(ThorsAnvil::Serialize::YamlPrinter& printer, ID const& object)   override
 *                  //virtual void readYaml(ThorsAnvil::Serialize::YamlParser& parser, ID& object)            override
 *
 *                  // generic version we simply stream the integer value.
 *                  static constexpr std::size_t sizeOfID = 12;
 *                  virtual  std::size_t getPrintSizeBson(ThorsAnvil::Serialize::BsonPrinter& printer, ID const& object) override
 *                  {
 *                      return sizeOfID;
 *                  }
 *                  virtual char getBsonByteMark()   override   {return '\x07';}
 *                  virtual void writeBson(ThorsAnvil::Serialize::BsonPrinter& printer, ID const& object)   override
 *                  {
 *                      printer.stream().write(reinterpret_cast<char const*>(&object.id), sizeof(object.id));
 *                      printer.stream().write("            ", sizeOfID - sizeof(object.id));
 *                  }
 *                  virtual void readBson(ThorsAnvil::Serialize::BsonParser& parser, char byteMarker, ID& object)             override
 *                  {
 *                      parser.stream().read(reinterpret_cast<char*>(&object.id), sizeof(object.id));
 *                      parser.stream().ignore(sizeOfID - sizeof(object.id));
 *                  }
 *              };
 *
 *              template<typename T>
 *              struct Flounder
 *              {
 *                  T   data;
 *              };
 *
 *              struct Transaction
 *              {
 *                  long        timeStamp;
 *                  int         amount;
 *                  TransType   type;
 *                  Transaction()
 *                      : Transaction(0, 0, TransType::Error)
 *                  {}
 *                  Transaction(long timeStamp, int amount, TransType type)
 *                      : timeStamp(timeStamp)
 *                      , amount(amount)
 *                      , type(type)
 *                  {}
 *              };
 *              class BankAccount
 *              {
 *                      friend ThorsAnvil::Serialize::Traits<OnLineBank::BankAccount>;
 *                      ID              id;
 *                      int             balance;
 *                      std::string     details;
 *                      bool            valid;
 *                  protected:
 *                      void update(int amount) {balance += amount;}
 *                  public:
 *                      BankAccount()
 *                          : BankAccount(-1, -1, "Bad", false)
 *                      {}
 *                      BankAccount(ID const& id, int balance, std::string const& details, bool valid)
 *                          : id(id)
 *                          , balance(balance)
 *                          , details(details)
 *                          , valid(valid)
 *                      {}
 *                      virtual ~BankAccount()  {}
 *                      int getAccountIdent()   {return id.id;}
 *                      int getBalance()        {return balance;}
 *                      bool isValid()          {return valid;}
 *                      ThorsAnvil_PolyMorphicSerializer(OnLineBank::BankAccount);
 *                      // Normal Methods
 *              };
 *              class CurrentAccount: public BankAccount
 *              {
 *                      friend ThorsAnvil::Serialize::Traits<OnLineBank::CurrentAccount>;
 *                      std::vector<Transaction>    actions;
 *                  public:
 *                      using BankAccount::BankAccount;
 *                      CurrentAccount() {}
 *                      ThorsAnvil_PolyMorphicSerializer(OnLineBank::CurrentAccount);
 *                      void addTransaction(long timeStamp, int amount, TransType type)
 *                      {
 *                          actions.emplace_back(timeStamp, amount, type);
 *                          switch (type)
 *                          {
 *                              case TransType::Withdraw:   update(-amount);break;
 *                              case TransType::Deposit:    update(amount);break;
 *                              case TransType::Correction: update(-getBalance() + amount);break;
 *                              default: break;
 *                          }
 *                      }
 *              };
 *              class DepositAccount: public BankAccount
 *              {
 *                      friend ThorsAnvil::Serialize::Traits<OnLineBank::DepositAccount>;
 *                      int withdrawlLimit;
 *                  public:
 *                      using BankAccount::BankAccount;
 *                      DepositAccount() {}
 *                      ThorsAnvil_PolyMorphicSerializer(OnLineBank::DepositAccount);
 *              };
 *          }
 *
 *          ThorsAnvil_MakeEnum(OnLineBank::TransType, Error, Deposit, Withdraw, Correction);
 *          ThorsAnvil_MakeTraitCustomSerialize(OnLineBank::ID, OnLineBank::SerializeID);
 *          ThorsAnvil_MakeTrait(OnLineBank::Transaction, timeStamp, amount, type);
 *          ThorsAnvil_Template_MakeTrait(1, OnLineBank::Flounder, data);
 *          ThorsAnvil_MakeTrait(OnLineBank::BankAccount, id, balance, details, valid);
 *          ThorsAnvil_ExpandTrait(OnLineBank::BankAccount, OnLineBank::CurrentAccount, actions);
 *          ThorsAnvil_ExpandTrait(OnLineBank::BankAccount, OnLineBank::DepositAccount, withdrawlLimit);
 *
 *      // Bank.cpp
 *
 *          ThorsAnvil_RegisterPolyMorphicType(OnLineBank::CurrentAccount);
 *          ThorsAnvil_RegisterPolyMorphicType(OnLineBank::DepositAccount);
 *
 */

#include "SerializeConfig.h"
#include "ThorsSerializerUtil.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"
#include <string>
#include <tuple>
#include <map>
#include <functional>
#include <memory>
#include <utility>
#include <type_traits>
#include <stdexcept>
#include <cstddef>
#include <cstring>
#include <numeric>

/*
 * Macros for counting the number of arguments
 * Currently set up for a max of 20.
 */
#define NUM_ARGS(...)          NUM_ARGS_(0, __VA_ARGS__, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 00, Ignore)
#define NUM_ARGS_(Zero, One, I1, I2, I3, I4 ,I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, I16, I17, I18, I19, I20, I21, I22, I23, I24, I25, I26, I27, I28, I29, I30, I31, I32, I33, I34, I35, I36, I37, I38, I39, I40, I41, I42, I43, A, ...)  A

/*
 * Macros to quote the parameter
 * Used below by the actions.
 */
#define QUOTE_(A)                       A
#define QUOTE(A)                        QUOTE_(#A)

/*
 * Macro to force expansion of a macro that takes two parameters.
 * Used by REP_*
 */
#define EXPAND_(Result)                 Result
#define EXPAND(Act, TC, P1, P2)         EXPAND_(Act(TC, P1, P2))
#define ALT_EXPAND_(Result)             Result
#define ALT_EXPAND(Act, Ex, Id)         EXPAND_(Act(Ex, Id))

/*
 * Macros that that applies the action `Act` (a two parameter macro)
 * To P1 (first argument)
 * and a list of second arguments (placing a comma between each macro).
 *
 * Because NUM_ARGS is limited to 20, This expansion is also limited to 20
 */
#define REP_N(Act, TC, P1, ...)             REP_OF_N(Act, TC, P1, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)
#define REP_OF_N(Act, TC, P1, Count, ...)   REP_OF_N_(Act, TC, P1, Count, __VA_ARGS__)
#define REP_OF_N_(Act, TC, P1, Count, ...)  REP_OF_ ## Count(Act, TC, P1, __VA_ARGS__)

#define REP_OF_43(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_42(Act, TC, P1, __VA_ARGS__)
#define REP_OF_42(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_41(Act, TC, P1, __VA_ARGS__)
#define REP_OF_41(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_40(Act, TC, P1, __VA_ARGS__)
#define REP_OF_40(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_39(Act, TC, P1, __VA_ARGS__)
#define REP_OF_39(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_38(Act, TC, P1, __VA_ARGS__)
#define REP_OF_38(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_37(Act, TC, P1, __VA_ARGS__)
#define REP_OF_37(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_36(Act, TC, P1, __VA_ARGS__)
#define REP_OF_36(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_35(Act, TC, P1, __VA_ARGS__)
#define REP_OF_35(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_34(Act, TC, P1, __VA_ARGS__)
#define REP_OF_34(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_33(Act, TC, P1, __VA_ARGS__)
#define REP_OF_33(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_32(Act, TC, P1, __VA_ARGS__)
#define REP_OF_32(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_31(Act, TC, P1, __VA_ARGS__)
#define REP_OF_31(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_30(Act, TC, P1, __VA_ARGS__)
#define REP_OF_30(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_29(Act, TC, P1, __VA_ARGS__)
#define REP_OF_29(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_28(Act, TC, P1, __VA_ARGS__)
#define REP_OF_28(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_27(Act, TC, P1, __VA_ARGS__)
#define REP_OF_27(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_26(Act, TC, P1, __VA_ARGS__)
#define REP_OF_26(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_25(Act, TC, P1, __VA_ARGS__)
#define REP_OF_25(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_24(Act, TC, P1, __VA_ARGS__)
#define REP_OF_24(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_23(Act, TC, P1, __VA_ARGS__)
#define REP_OF_23(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_22(Act, TC, P1, __VA_ARGS__)
#define REP_OF_22(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_21(Act, TC, P1, __VA_ARGS__)
#define REP_OF_21(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_20(Act, TC, P1, __VA_ARGS__)
#define REP_OF_20(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_19(Act, TC, P1, __VA_ARGS__)
#define REP_OF_19(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_18(Act, TC, P1, __VA_ARGS__)
#define REP_OF_18(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_17(Act, TC, P1, __VA_ARGS__)
#define REP_OF_17(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_16(Act, TC, P1, __VA_ARGS__)
#define REP_OF_16(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_15(Act, TC, P1, __VA_ARGS__)
#define REP_OF_15(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_14(Act, TC, P1, __VA_ARGS__)
#define REP_OF_14(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_13(Act, TC, P1, __VA_ARGS__)
#define REP_OF_13(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_12(Act, TC, P1, __VA_ARGS__)
#define REP_OF_12(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_11(Act, TC, P1, __VA_ARGS__)
#define REP_OF_11(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_10(Act, TC, P1, __VA_ARGS__)
#define REP_OF_10(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_9(Act, TC, P1, __VA_ARGS__)
#define REP_OF_9(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_8(Act, TC, P1, __VA_ARGS__)
#define REP_OF_8(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_7(Act, TC, P1, __VA_ARGS__)
#define REP_OF_7(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_6(Act, TC, P1, __VA_ARGS__)
#define REP_OF_6(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_5(Act, TC, P1, __VA_ARGS__)
#define REP_OF_5(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_4(Act, TC, P1, __VA_ARGS__)
#define REP_OF_4(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_3(Act, TC, P1, __VA_ARGS__)
#define REP_OF_3(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1 ,P2), REP_OF_2(Act, TC, P1, __VA_ARGS__)
#define REP_OF_2(Act, TC, P1, P2, ...)     EXPAND(Act, TC, P1, P2), REP_OF_1(Act, TC, P1, __VA_ARGS__)
#define REP_OF_1(Act, TC, P1, P2, One)     EXPAND(Act, TC, P1, P2)
#define REP_OF_00(Act, TC, P1, One)         LAST_ ## Act(TC, P1)

#define ALT_REP_OF_N(Act, E, P, S, Count)  ALT_REP_OF_N_(Act, E, P, S, Count)
#define ALT_REP_OF_N_(Act, E, P, S, Count) ALT_REP_OF_ ## Count(Act, E, P, S)

#define ALT_REP_OF_43(Act, E, P, S)     P ALT_EXPAND(Act, E, 43), ALT_REP_OF_42(Act, E,  , S)
#define ALT_REP_OF_42(Act, E, P, S)     P ALT_EXPAND(Act, E, 42), ALT_REP_OF_41(Act, E,  , S)
#define ALT_REP_OF_41(Act, E, P, S)     P ALT_EXPAND(Act, E, 41), ALT_REP_OF_40(Act, E,  , S)
#define ALT_REP_OF_40(Act, E, P, S)     P ALT_EXPAND(Act, E, 40), ALT_REP_OF_39(Act, E,  , S)
#define ALT_REP_OF_39(Act, E, P, S)     P ALT_EXPAND(Act, E, 39), ALT_REP_OF_38(Act, E,  , S)
#define ALT_REP_OF_38(Act, E, P, S)     P ALT_EXPAND(Act, E, 38), ALT_REP_OF_37(Act, E,  , S)
#define ALT_REP_OF_37(Act, E, P, S)     P ALT_EXPAND(Act, E, 37), ALT_REP_OF_36(Act, E,  , S)
#define ALT_REP_OF_36(Act, E, P, S)     P ALT_EXPAND(Act, E, 36), ALT_REP_OF_35(Act, E,  , S)
#define ALT_REP_OF_35(Act, E, P, S)     P ALT_EXPAND(Act, E, 35), ALT_REP_OF_34(Act, E,  , S)
#define ALT_REP_OF_34(Act, E, P, S)     P ALT_EXPAND(Act, E, 34), ALT_REP_OF_33(Act, E,  , S)
#define ALT_REP_OF_33(Act, E, P, S)     P ALT_EXPAND(Act, E, 33), ALT_REP_OF_32(Act, E,  , S)
#define ALT_REP_OF_32(Act, E, P, S)     P ALT_EXPAND(Act, E, 32), ALT_REP_OF_31(Act, E,  , S)
#define ALT_REP_OF_31(Act, E, P, S)     P ALT_EXPAND(Act, E, 31), ALT_REP_OF_30(Act, E,  , S)
#define ALT_REP_OF_30(Act, E, P, S)     P ALT_EXPAND(Act, E, 30), ALT_REP_OF_29(Act, E,  , S)
#define ALT_REP_OF_29(Act, E, P, S)     P ALT_EXPAND(Act, E, 29), ALT_REP_OF_28(Act, E,  , S)
#define ALT_REP_OF_28(Act, E, P, S)     P ALT_EXPAND(Act, E, 28), ALT_REP_OF_27(Act, E,  , S)
#define ALT_REP_OF_27(Act, E, P, S)     P ALT_EXPAND(Act, E, 27), ALT_REP_OF_26(Act, E,  , S)
#define ALT_REP_OF_26(Act, E, P, S)     P ALT_EXPAND(Act, E, 26), ALT_REP_OF_25(Act, E,  , S)
#define ALT_REP_OF_25(Act, E, P, S)     P ALT_EXPAND(Act, E, 25), ALT_REP_OF_24(Act, E,  , S)
#define ALT_REP_OF_24(Act, E, P, S)     P ALT_EXPAND(Act, E, 24), ALT_REP_OF_23(Act, E,  , S)
#define ALT_REP_OF_23(Act, E, P, S)     P ALT_EXPAND(Act, E, 23), ALT_REP_OF_22(Act, E,  , S)
#define ALT_REP_OF_22(Act, E, P, S)     P ALT_EXPAND(Act, E, 22), ALT_REP_OF_21(Act, E,  , S)
#define ALT_REP_OF_21(Act, E, P, S)     P ALT_EXPAND(Act, E, 21), ALT_REP_OF_20(Act, E,  , S)
#define ALT_REP_OF_20(Act, E, P, S)     P ALT_EXPAND(Act, E, 20), ALT_REP_OF_19(Act, E,  , S)
#define ALT_REP_OF_19(Act, E, P, S)     P ALT_EXPAND(Act, E, 19), ALT_REP_OF_18(Act, E,  , S)
#define ALT_REP_OF_18(Act, E, P, S)     P ALT_EXPAND(Act, E, 18), ALT_REP_OF_17(Act, E,  , S)
#define ALT_REP_OF_17(Act, E, P, S)     P ALT_EXPAND(Act, E, 17), ALT_REP_OF_16(Act, E,  , S)
#define ALT_REP_OF_16(Act, E, P, S)     P ALT_EXPAND(Act, E, 16), ALT_REP_OF_15(Act, E,  , S)
#define ALT_REP_OF_15(Act, E, P, S)     P ALT_EXPAND(Act, E, 15), ALT_REP_OF_14(Act, E,  , S)
#define ALT_REP_OF_14(Act, E, P, S)     P ALT_EXPAND(Act, E, 14), ALT_REP_OF_13(Act, E,  , S)
#define ALT_REP_OF_13(Act, E, P, S)     P ALT_EXPAND(Act, E, 13), ALT_REP_OF_12(Act, E,  , S)
#define ALT_REP_OF_12(Act, E, P, S)     P ALT_EXPAND(Act, E, 12), ALT_REP_OF_11(Act, E,  , S)
#define ALT_REP_OF_11(Act, E, P, S)     P ALT_EXPAND(Act, E, 11), ALT_REP_OF_10(Act, E,  , S)
#define ALT_REP_OF_10(Act, E, P, S)     P ALT_EXPAND(Act, E, 10), ALT_REP_OF_9(Act, E,  , S)
#define ALT_REP_OF_9(Act, E, P, S)     P ALT_EXPAND(Act, E, 9), ALT_REP_OF_8(Act, E,  , S)
#define ALT_REP_OF_8(Act, E, P, S)     P ALT_EXPAND(Act, E, 8), ALT_REP_OF_7(Act, E,  , S)
#define ALT_REP_OF_7(Act, E, P, S)     P ALT_EXPAND(Act, E, 7), ALT_REP_OF_6(Act, E,  , S)
#define ALT_REP_OF_6(Act, E, P, S)     P ALT_EXPAND(Act, E, 6), ALT_REP_OF_5(Act, E,  , S)
#define ALT_REP_OF_5(Act, E, P, S)     P ALT_EXPAND(Act, E, 5), ALT_REP_OF_4(Act, E,  , S)
#define ALT_REP_OF_4(Act, E, P, S)     P ALT_EXPAND(Act, E, 4), ALT_REP_OF_3(Act, E,  , S)
#define ALT_REP_OF_3(Act, E, P, S)     P ALT_EXPAND(Act, E, 3), ALT_REP_OF_2(Act, E,  , S)
#define ALT_REP_OF_2(Act, E, P, S)     P ALT_EXPAND(Act, E, 2), ALT_REP_OF_1(Act, E,  , S)
#define ALT_REP_OF_1(Act, E, P, S)     P ALT_EXPAND(Act, E, 1) S
#define ALT_REP_OF_00(Act, E, P, S)     LAST_ ## Act(E, 00)

/*
 * The actions we apply with REP_*
 *
 * THOR_TYPEACTION:      Declares a type to hold the name and a pointer to the internal object.
 * THOR_VALUEACTION:     Declares an initialization of the Type putting the name and the pointer
 *                  into the object
 */
#define BUILDTEMPLATETYPEPARAM(Act, Count)      ALT_REP_OF_N(Act, ,  ,  , Count)
#define BUILDTEMPLATETYPEVALUE(Act, Count)      ALT_REP_OF_N(Act, , <, >, Count)


#define THOR_TYPEACTION(TC, Type, Member)       std::pair<char const*, decltype(&Type BUILDTEMPLATETYPEVALUE(THOR_TYPENAMEVALUEACTION, TC) ::Member)>
#define THOR_VALUEACTION(TC, Type, Member)      { QUOTE(Member), &Type BUILDTEMPLATETYPEVALUE(THOR_TYPENAMEVALUEACTION, TC) ::Member }
#define THOR_NAMEACTION(TC, Type, Member)       { Type::Member, #Member ## s}
#define LAST_THOR_TYPEACTION(TC, Type)
#define LAST_THOR_VALUEACTION(TC, Type)
#define LAST_THOR_NAMEACTION(TC, Type)

#define THOR_TYPENAMEPARAMACTION(Ex, Id)        typename T ## Id
#define THOR_TYPENAMEVALUEACTION(Ex, Id)        T ## Id
#define THOR_TYPE_INT_VALUE(Ex, Id)             ThorsAnvil::Serialize::Fake ## Id
#define THOR_CHECK_ASSERT(Ex, Id)
#define LAST_THOR_TYPENAMEPARAMACTION(Ex, Id)
#define LAST_THOR_TYPENAMEVALUEACTION(Ex, Id)
#define LAST_THOR_TYPE_INT_VALUE(Ex, Id)
#define LAST_THOR_CHECK_ASSERT(Ex, Id)          DO_ASSERT(Ex)


#define THOR_MERGE_LABEL_NAME(Pre, Post)        Pre ## Post
#define THOR_UNIQUE_LABEL(Line)                 THOR_MERGE_LABEL_NAME(thorUniqueName, Line)
#define THOR_UNIQUE_NAME                        THOR_UNIQUE_LABEL(__COUNTER__)

/*
 * Defines a trait for a user defined type.
 * Lists the members of the type that can be serialized.
 */
#define DO_ASSERT(DataType)             DO_ASSERT_WITH_TEMPLATE(DataType, 00)
#define DO_ASSERT_WITH_TEMPLATE(DataType, Count)                        \
static_assert(                                                          \
    ::ThorsAnvil::Serialize::Traits<DataType BUILDTEMPLATETYPEVALUE(THOR_TYPE_INT_VALUE, Count) >::type != ThorsAnvil::Serialize::TraitType::Invalid,   \
    "The macro ThorsAnvil_MakeTrait must be used outside all namespace."\
)

#define ThorsAnvil_PointerAllocator(DataType, ActionObj)                \
namespace ThorsAnvil { namespace Serialize {                            \
template<>                                                              \
class Traits<DataType*>                                                 \
{                                                                       \
    public:                                                             \
        static constexpr TraitType type = TraitType::Pointer;           \
        static DataType* alloc()    {return ActionObj::alloc();}        \
        static void release(T* p)   {ActionObj::release(p);}            \
};                                                                      \
}}

#define ThorsAnvil_MakeOverride(...)                                    \
    ThorsAnvil_MakeOverride_Base(00,     __VA_ARGS__, {"", ""})
#define ThorsAnvil_Template_MakeOverride(Count, ...)                    \
    ThorsAnvil_MakeOverride_Base(Count, __VA_ARGS__, {"", ""})


#define ThorsAnvil_MakeOverride_Base(Count, DataType, ...)              \
namespace ThorsAnvil { namespace Serialize {                            \
template<BUILDTEMPLATETYPEPARAM(THOR_TYPENAMEPARAMACTION, Count)>       \
class Override<DataType BUILDTEMPLATETYPEVALUE(THOR_TYPENAMEVALUEACTION, Count) > \
{                                                                       \
    public:                                                             \
        static char const* nameOverride(char const* name)               \
        {                                                               \
            static auto hashCString = [](char const* str)               \
            {                                                           \
                std::hash<std::string_view>  viewHash;                  \
                return viewHash(std::string_view(str));                 \
            };                                                          \
            static auto cmpCString = [](char const* lhs, char const* rhs)\
            {                                                           \
                return std::strcmp(lhs, rhs) == 0;                      \
            };                                                          \
            using OverrideMap = std::unordered_map<char const*, char const*, decltype(hashCString), decltype(cmpCString)>;   \
            static OverrideMap overrideMap =                            \
            {                                                           \
                {__VA_ARGS__},                                          \
                101 /* Prime */,                                        \
                hashCString,                                            \
                cmpCString                                              \
            };                                                          \
            auto find = overrideMap.find(name);                         \
            if (find == overrideMap.end()) {                            \
                return name;                                            \
            }                                                           \
            return find->second;                                        \
        }                                                               \
};                                                                      \
}}                                                                      \
static_assert(true, "")


#define ThorsAnvil_MakeFilter(DataType, member)                         \
    ThorsAnvil_MakeFilter_Base(00,    DataType, member)
#define ThorsAnvil_Template_MakeFilter(Count, DataType, member)         \
    ThorsAnvil_MakeFilter_Base(Count, DataType, member)


#define ThorsAnvil_MakeFilter_Base(Count, DataType, member)             \
namespace ThorsAnvil { namespace Serialize {                            \
template<BUILDTEMPLATETYPEPARAM(THOR_TYPENAMEPARAMACTION, Count)>       \
class Filter<DataType BUILDTEMPLATETYPEVALUE(THOR_TYPENAMEVALUEACTION, Count) > \
{                                                                       \
    public:                                                             \
        static bool filter(DataType BUILDTEMPLATETYPEVALUE(THOR_TYPENAMEVALUEACTION, Count) const& object, char const* name)    \
        {                                                               \
            auto find = object.member.find(name);                       \
            return find == object.member.end() ? true : find->second;   \
        }                                                               \
};                                                                      \
}}                                                                      \
static_assert(true, "")


#define ThorsAnvil_MakeTrait_Base(ParentType, TType, Count, DataType, ...)  \
namespace ThorsAnvil { namespace Serialize {                            \
template<BUILDTEMPLATETYPEPARAM(THOR_TYPENAMEPARAMACTION, Count)>       \
class Traits<DataType BUILDTEMPLATETYPEVALUE(THOR_TYPENAMEVALUEACTION, Count) > \
{                                                                       \
    public:                                                             \
        static constexpr TraitType type = TraitType::TType;             \
        ParentType                                                      \
        using MyType = DataType BUILDTEMPLATETYPEVALUE(THOR_TYPENAMEVALUEACTION, Count); \
                                                                        \
        using Members = std::tuple<                                     \
                        REP_N(THOR_TYPEACTION, Count, DataType, __VA_ARGS__)        \
                                    >;                                  \
                                                                        \
        static Members const& getMembers()                              \
        {                                                               \
            static constexpr Members members{                           \
                        REP_N(THOR_VALUEACTION, Count, DataType, __VA_ARGS__)       \
                                            };                          \
            return members;                                             \
        }                                                               \
                                                                        \
        template<typename M>                                            \
        static std::size_t addSizeOneMember(PrinterInterface& printer, MyType const& /*object*/, M* staticObjPtr) \
        {                                                               \
            using MemberType    = std::decay_t<M>;                      \
            if (staticObjPtr)                                           \
            {                                                           \
                return Traits<MemberType>::getPrintSize(printer, *staticObjPtr, false);\
            }                                                           \
            return printer.getSizeNull();                               \
        }                                                               \
        template<typename M>                                            \
        static std::size_t addSizeOneMember(PrinterInterface& printer, MyType const& object, M MyType::* memPtr) \
        {                                                               \
            using MemberTypeDec = decltype(object.*memPtr);             \
            using MemberType    = std::decay_t<MemberTypeDec>;          \
            return Traits<MemberType>::getPrintSize(printer, object.*memPtr, false);\
        }                                                               \
                                                                        \
        template<typename M>                                            \
        static std::pair<std::size_t, std::size_t> addSizeEachMemberItem(PrinterInterface& printer, MyType const& object, M item) \
        {                                                               \
            if (!Filter<MyType>::filter(object, item.first)) {          \
                return std::make_pair(0UL,0UL);                         \
            }                                                           \
            auto partSize   = addSizeOneMember(printer, object, item.second);           \
            auto nameSize   = std::strlen(Override<MyType>::nameOverride(item.first));  \
            return std::make_pair(partSize + nameSize, 1);              \
        }                                                               \
        template<std::size_t... Seq>                                    \
        static std::pair<std::size_t, std::size_t> addSizeEachMember(PrinterInterface& printer, MyType const& object, std::index_sequence<Seq...> const&) \
        {                                                               \
            Members const& members = getMembers();                      \
            std::initializer_list<std::pair<std::size_t, std::size_t>>  sizeData = {    \
                std::make_pair(std::size_t{0}, std::size_t{0}),                         \
                addSizeEachMemberItem(printer, object, std::get<Seq>(members))...       \
            };                                                          \
            return std::accumulate(std::begin(sizeData), std::end(sizeData), std::make_pair(0UL, 0UL),                                  \
                                   [](auto lhs, auto rhs){return std::make_pair(lhs.first + rhs.first, lhs.second + rhs.second);});     \
        }                                                               \
                                                                        \
        static std::size_t getPrintSizeTotal(PrinterInterface& printer, MyType const& object, std::size_t& count, std::size_t& memberSize)\
        {                                                               \
            auto r = addSizeEachMember(printer, object, std::make_index_sequence<std::tuple_size_v<Members>>());\
            memberSize  += r.first;                                     \
            count       += r.second;                                    \
                                                                        \
            CalcSizeHelper<MyType>  calcHelper;                         \
            return calcHelper.getPrintSize(printer, object, count, memberSize);\
        }                                                               \
                                                                        \
        static std::size_t getPrintSize(PrinterInterface& printer, MyType const& object, bool poly)\
        {                                                               \
            return tryGetPolyMorphicPrintSize(printer, object, poly, 0);\
        }                                                               \
};                                                                      \
}}                                                                      \
DO_ASSERT_WITH_TEMPLATE(DataType, Count)

#define ThorsAnvil_RegisterPolyMorphicType_Internal(DataType, ...)      \
    ThorsAnvil_RegisterPolyMorphicType(DataType)

#define ThorsAnvil_RegisterPolyMorphicType(DataType)                    \
namespace ThorsAnvil { namespace Serialize {                            \
namespace                                                               \
{                                                                       \
    ThorsAnvil_InitPolyMorphicType<DataType>   THOR_UNIQUE_NAME ( # DataType); \
}                                                                       \
}}

#define ThorsAnvil_Parent(Count, ParentType, DataType, ...)             \
        using Parent = ParentType BUILDTEMPLATETYPEVALUE(THOR_TYPENAMEVALUEACTION, Count); \
        using Root   = typename GetRootType<ParentType BUILDTEMPLATETYPEVALUE(THOR_TYPENAMEVALUEACTION, Count) >::Root;

#define ThorsAnvil_Template_MakeTrait(Count, ...)                       \
    ThorsAnvil_MakeTrait_Base( , Map, Count, __VA_ARGS__, 1);           \
    static_assert(true, "")

#define ThorsAnvil_MakeTrait(...)                                       \
    ThorsAnvil_MakeTrait_Base( , Map, 00, __VA_ARGS__, 1);              \
    ThorsAnvil_RegisterPolyMorphicType_Internal(__VA_ARGS__, 1)         \
    static_assert(true, "")

#define ThorsAnvil_MakeTraitCustom(DataType)                            \
namespace ThorsAnvil { namespace Serialize {                            \
template<>                                                              \
class Traits<DataType>                                                  \
{                                                                       \
    public:                                                             \
    static constexpr TraitType type = TraitType::Custom_Depricated;     \
    static std::size_t getPrintSize(PrinterInterface& printer, DataType const& value, bool)\
    {                                                                   \
        return tryGetSizeFromSerializeType(printer, value, 0);          \
    }                                                                   \
};                                                                      \
}}                                                                      \
DO_ASSERT(DataType)

#define ThorsAnvil_MakeTraitCustomSerialize(DataType, SerializeType)    \
namespace ThorsAnvil { namespace Serialize {                            \
template<>                                                              \
class Traits<DataType>                                                  \
{                                                                       \
    public:                                                             \
    static constexpr TraitType type = TraitType::Custom_Serialize;      \
    using SerializingType   = SerializeType;                            \
    static std::size_t getPrintSize(PrinterInterface& printer, DataType const& object, bool)\
    {                                                                   \
        SerializingType info;                                           \
        switch (printer.formatType())                                   \
        {                                                               \
            case FormatType::Bson:  return info.getPrintSizeBson(dynamic_cast<BsonPrinter&>(printer), object);\
            case FormatType::Json:  /* Fall Through */                  \
            case FormatType::Yaml:  /* Fall Through */                  \
            default:                                                    \
            {                                                           \
                ThorsLogAndThrowCritical("ThorsAnivl::Seriaizlize::Traits<DataType>",   \
                                         "getPrintSize",                \
                                         "Should not get here");        \
            }                                                           \
        }                                                               \
    }                                                                   \
};                                                                      \
}}                                                                      \
DO_ASSERT(DataType)

#define ThorsAnvil_Template_ExpandTrait(Count, ParentType, ...)         \
    ThorsAnvil_MakeTrait_Base(ThorsAnvil_Parent(00, ParentType, __VA_ARGS__, 1), Parent, Count, __VA_ARGS__, 1); \
    static_assert(true, "")

#define ThorsAnvil_Template_ExpandTemplate(Count, ParentCount, ParentType, ...)         \
    ThorsAnvil_MakeTrait_Base(ThorsAnvil_Parent(ParentCount, ParentType, __VA_ARGS__, 1), Parent, Count, __VA_ARGS__, 1); \
    static_assert(true, "")

#define ThorsAnvil_ExpandTrait(ParentType, ...)                     ThorsAnvil_ExpandTrait_Base(ParentType, __VA_ARGS__, 1)
#define ThorsAnvil_ExpandTrait_Base(ParentType, DataType, ...)          \
    static_assert(                                                      \
        std::is_base_of<typename ThorsAnvil::Serialize::GetPrimaryParentType<ParentType>::type, DataType>::value,                  \
        "ParentType must be a base class of DataType");                 \
    static_assert(                                                      \
        ::ThorsAnvil::Serialize::Traits<ParentType>::type != ThorsAnvil::Serialize::TraitType::Invalid, \
        "Parent type must have Serialization Traits defined"            \
    );                                                                  \
    ThorsAnvil_MakeTrait_Base(ThorsAnvil_Parent(00, ParentType, DataType, __VA_ARGS__), Parent, 00, DataType, __VA_ARGS__); \
    ThorsAnvil_RegisterPolyMorphicType_Internal(DataType, 1)            \
    static_assert(true, "")

#include <magic_enum.hpp>
namespace ThorsAnvil
{
    namespace Serialize
    {
template<typename EnumName>
class Traits<EnumName, std::enable_if_t<std::is_enum<EnumName>::value>>
{
    public:
        static constexpr    TraitType       type = TraitType::Enum;
        static std::size_t getSize()
        {
            return magic_enum::enum_count<EnumName>();
        }
        static EnumName getValue(std::string const& val, std::string const&)
        {
            auto enumDecode = magic_enum::enum_cast<EnumName>(val);
            if (enumDecode.has_value())
            {
                return enumDecode.value();
            }
            ThorsLogAndThrow("ThorsAnvil::Serialize::Traits<EnumName>",
                             "getValue",
                             "Invalid Enum Value");
        }
        static std::size_t getPrintSize(PrinterInterface& printer, EnumName const& value, bool)
        {
            auto enumName = magic_enum::enum_name(value);
            return printer.getSizeValue(enumName);
        }
        static void serializeForBlock(PrinterInterface& printer, EnumName const& object)
        {
            auto findValue = magic_enum::enum_name(object);
            if (findValue == "")
            {
                ThorsLogAndThrow("ThorsAnvil::Serialize::Traits<EnumName(With Magic)>",
                                 "serializeForBlock",
                                 "Invalid Enum Value");
            }
            printer.addValue(magic_enum::enum_name(object));
        }
};
    }
}

#include <map>
#include <string>
#define ThorsAnvil_MakeEnum(EnumName, ...)                              \
namespace ThorsAnvil { namespace Serialize {                            \
template<>                                                              \
class Traits<EnumName>                                                  \
{                                                                       \
    public:                                                             \
        static constexpr    TraitType       type = TraitType::Enum;     \
        static std::map<EnumName, std::string> const& getValues()       \
        {                                                               \
            using std::string_literals::operator""s;                    \
            static const std::map<EnumName, std::string> values =       \
            {                                                           \
                REP_N(THOR_NAMEACTION, 0, EnumName, __VA_ARGS__, 1)     \
            };                                                          \
            return values;                                              \
        }                                                               \
        static std::size_t getSize()                                    \
        {                                                               \
            return NUM_ARGS(__VA_ARGS__, 1);                            \
        }                                                               \
        static EnumName getValue(std::string const& val, std::string const&) \
        {                                                               \
            std::map<EnumName, std::string> const& values = getValues();\
            for (auto const& value: values)                             \
            {                                                           \
                if (val == value.second) {                              \
                    return value.first;                                 \
                }                                                       \
            }                                                           \
            ThorsLogAndThrow("ThorsAnvil::Serialize::Traits<EnumName>", \
                             "getValue",                                \
                             "Invalid Enum Value");                     \
        }                                                               \
        static std::size_t getPrintSize(PrinterInterface& printer, EnumName const& value, bool)\
        {                                                               \
            auto find = getValues().find(value);                        \
            return printer.getSizeValue(find->second);                  \
        }                                                               \
        static void serializeForBlock(PrinterInterface& printer, EnumName const& object) \
        {                                                               \
            auto values = getValues();                                  \
            auto find = values.find(object);                            \
            if (find == values.end()) {                                 \
                ThorsLogAndThrow("ThorsAnvil::Serialize::Traits<EnumName>", \
                                 "serializeForBlock",                       \
                                 "Invalid Enum Value");                     \
            }                                                           \
            printer.addValue(getValues().find(object)->second);         \
        }                                                               \
};                                                                      \
}}                                                                      \
DO_ASSERT(EnumName)

// This type is not useful for JSON or other serialization
// But it can be useful to dump an enum that is a flag based enum.
// Its interface is not advertised above deliberately
#define ThorsAnvil_MakeEnumFlag(EnumName, ...)                          \
inline EnumName operator&(EnumName lhs, EnumName rhs){return static_cast<EnumName>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));}\
inline EnumName operator|(EnumName lhs, EnumName rhs){return static_cast<EnumName>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));}\
inline EnumName operator^(EnumName lhs, EnumName rhs){return static_cast<EnumName>(static_cast<unsigned int>(lhs) ^ static_cast<unsigned int>(rhs));}\
inline EnumName operator&=(EnumName& lhs, EnumName rhs){lhs = lhs & rhs; return lhs;}\
inline EnumName operator|=(EnumName& lhs, EnumName rhs){lhs = lhs | rhs; return lhs;}\
inline EnumName operator^=(EnumName& lhs, EnumName rhs){lhs = lhs ^ rhs; return lhs;}\
inline std::ostream& operator<<(std::ostream& stream, EnumName const& value)    \
{                                                                               \
    if (value == EnumName::empty)                                               \
    {                                                                           \
        stream << "empty";                                                      \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        using std::string_literals::operator""s;                                \
        static const std::map<EnumName, std::string> values =                   \
        {                                                                       \
            REP_N(THOR_NAMEACTION, 0, EnumName, __VA_ARGS__, 1)                 \
        };                                                                      \
        bool                first = true;                                       \
        for (int loop = 0; loop < 32; ++loop)                                   \
        {                                                                       \
            EnumName flag = static_cast<EnumName>(1 << loop);                   \
            if ((value & flag) != EnumName::empty)                              \
            {                                                                   \
                if (!first)                                                     \
                {                                                               \
                    stream << " | ";                                            \
                }                                                               \
                first = false;                                                  \
                stream << values.find(flag)->second;                            \
            }                                                                   \
        }                                                                       \
    }                                                                           \
    return stream;                                                              \
}                                                                               \
static_assert(true)

/*
 * Defined the virtual function needed by tryPrintPolyMorphicObject()
 */
#define ThorsAnvil_PolyMorphicSerializer(Type)                                              \
    virtual void printPolyMorphicObject(ThorsAnvil::Serialize::Serializer&         parent,  \
                                       ThorsAnvil::Serialize::PrinterInterface&    printer) \
    {                                                                                       \
        ThorsAnvil::Serialize::printPolyMorphicObject<Type>(parent, printer, *this);        \
    }                                                                                       \
    virtual void parsePolyMorphicObject(ThorsAnvil::Serialize::DeSerializer&       parent,  \
                                       ThorsAnvil::Serialize::ParserInterface&     parser)  \
    {                                                                                       \
        ThorsAnvil::Serialize::parsePolyMorphicObject<Type>(parent, parser, *this);         \
    }                                                                                       \
    virtual std::size_t getPolyMorphicPrintSize(ThorsAnvil::Serialize::PrinterInterface& printer) const \
    {                                                                                       \
        std::size_t count = 1;                                                              \
        std::size_t memberSize = (printer.config.polymorphicMarker.size() + printer.getSizeValue(std::string(polyMorphicSerializerName())));\
                                                                                            \
        return getNormalPrintSize(printer, *this, count, memberSize);                       \
    }                                                                                       \
    static constexpr char const* polyMorphicSerializerName()                                \
    {                                                                                       \
        return #Type;                                                                       \
    }

#define ThorsAnvil_PolyMorphicSerializerWithOverride(Type)                                  \
    virtual void printPolyMorphicObject(ThorsAnvil::Serialize::Serializer&         parent,  \
                                       ThorsAnvil::Serialize::PrinterInterface&    printer) override \
    {                                                                                       \
        ThorsAnvil::Serialize::printPolyMorphicObject<Type>(parent, printer, *this);        \
    }                                                                                       \
    virtual void parsePolyMorphicObject(ThorsAnvil::Serialize::DeSerializer&       parent,  \
                                       ThorsAnvil::Serialize::ParserInterface&     parser)  override \
    {                                                                                       \
        ThorsAnvil::Serialize::parsePolyMorphicObject<Type>(parent, parser, *this);         \
    }                                                                                       \
    virtual std::size_t getPolyMorphicPrintSize(ThorsAnvil::Serialize::PrinterInterface& printer) const override \
    {                                                                                       \
        std::size_t count = 1;                                                              \
        std::size_t memberSize = (printer.config.polymorphicMarker.size() + printer.getSizeValue(std::string(polyMorphicSerializerName())));\
                                                                                            \
        return getNormalPrintSize(printer, *this, count, memberSize);                       \
    }                                                                                       \
    static constexpr char const* polyMorphicSerializerName()                                \
    {                                                                                       \
        return #Type;                                                                       \
    }


namespace ThorsAnvil
{
    namespace Serialize
    {

/*
 * The traits type.
 * Specialized for each type we want to serialize
 */
template<typename T, typename SFINE>
class Traits
{
    public:
        // By default if a traits type is not defined for a type
        // You get this default implementation.
        // This just says that it is invalid to serialize this type
        // static_asserts sprinkled through the code will provide
        // appropriate error messages based on this being invalid.
        static constexpr TraitType type = TraitType::Invalid;

        // For types that are Map/Array/Parent we can also define
        // members that need be serialized via types. To achieve this
        // they  define a static getMembers() function.
        // static Members const& getMembers()
        //
        // I would have made this a type declaration but I could
        // not get it to bind correctly without an explicit
        // instantiation (if you know how please let me know).
        //
        // So I use a static member function with a static variable
        // which can be defined in-line within the traits class and
        // does not need a separate declaration in a compilation unit.
};
template<typename T>
class Override
{
    public:
        static char const* nameOverride(char const* name) {return name;}
};

template<typename T>
class Filter
{
    public:
        static constexpr bool filter(T const& /*object*/, char const* /*name*/)   {return true;}
};

/*
 * For object that are serialized as Json Array
 * we use this object to get the size of the array.
 *
 * The default action is simply to call size() on
 * the object before printing.
 */
template<typename T>
class SerializeArraySize
{
    public:
        static std::size_t size(T const& object)
        {
            return object.size();
        }
};

template<typename T, typename R = T>
struct GetRootType
{
    using Root = R;
};
template<typename T>
struct GetRootType<T, typename Traits<T>::Root>
{
    using Root = typename Traits<T>::Root;
};
template<typename T>
struct GetAllocationType
{
    using AllocType = T;
};
template<typename T>
struct GetAllocationType<std::unique_ptr<T>>
{
    using AllocType = T;
};
#ifdef  SCGRROT_SHARED_PTR_SUPPRT
/*
 * This will work for shared pointers at a very basic level.
 *
 * But is conditionally included as it does not support one object used by multiple shared pointer
 * in the same object. Serializing and de-serializing will result in multiple versions of the
 * object in the new object.
 */
template<typename T>
struct GetAllocationType<std::shared_ptr<T>>
{
    using AllocType = T;
};
#endif

/*
 */
class PolyMorphicRegistry
{
    static std::map<std::string, std::function<void*()>>& getContainer()
    {
        static std::map<std::string, std::function<void*()>>    polyAllocContainer;
        return polyAllocContainer;
    }

    public:
        static std::function<void*()>& getAllocator(std::string const& name)
        {
            return getContainer()[name];
        }
        template<typename T>
        static T* getNamedTypeConvertedTo(std::string const& name)
        {
            using AllocType     = typename GetAllocationType<T>::AllocType;

            auto     cont       = getContainer();
            auto     find       = cont.find(name);
            if (find == cont.end())
            {
                ThorsLogAndThrow("ThorsAnvil::Serialize::PolyMorphicRegistry",
                                 "getNamedTypeConvertedTo",
                                 "Non polymorphic type");
            }
            void*       data        = find->second();
            AllocType*  dataBase    = reinterpret_cast<AllocType*>(data);

            using ReturnType    = T*;
            return ReturnType{dataBase};
        }
};

template <typename T>
class HasPolyMorphicObjectMarker
{
    using One = char;
    using Two = long;

    template <typename C> static One test( decltype(&C::parsePolyMorphicObject) );
    template <typename C> static Two test(...);

public:
    enum { Value = sizeof(test<T>(nullptr)) == sizeof(char) };
};


template<typename T, bool Poly = HasPolyMorphicObjectMarker<T>::Value>
struct ThorsAnvil_InitPolyMorphicType;


template<typename T>
struct ThorsAnvil_InitPolyMorphicType<T, true>
{
    ThorsAnvil_InitPolyMorphicType(char const* name)
    {
        PolyMorphicRegistry::getAllocator(name) =
            []() -> void*
            {
                using Root = typename GetRootType<T>::Root;
                return dynamic_cast<Root*>(Traits<T*>::alloc());
            };
    }
};

template<typename T>
struct ThorsAnvil_InitPolyMorphicType<T, false>
{
    ThorsAnvil_InitPolyMorphicType(char const*){}
};

struct Fake1{};
struct Fake2{};
struct Fake3{};
struct Fake4{};
struct Fake5{};
struct Fake6{};
struct Fake7{};
struct Fake8{};
struct Fake9{};
struct Fake10{};
struct Fake11{};
struct Fake12{};
struct Fake13{};
struct Fake14{};
struct Fake15{};
struct Fake16{};
struct Fake17{};
struct Fake18{};
struct Fake19{};
struct Fake20{};
struct Fake21{};
struct Fake22{};
struct Fake23{};
struct Fake24{};
struct Fake25{};
struct Fake26{};
struct Fake27{};
struct Fake28{};
struct Fake29{};
struct Fake30{};
struct Fake31{};
struct Fake32{};
struct Fake33{};
struct Fake34{};
struct Fake35{};
struct Fake36{};
struct Fake37{};
struct Fake38{};
struct Fake39{};
struct Fake40{};
struct Fake41{};
struct Fake42{};
struct Fake43{};
struct Fake44{};
struct Fake45{};
struct Fake46{};
struct Fake47{};
struct Fake48{};
struct Fake49{};
struct Fake50{};
struct Fake51{};
struct Fake52{};
struct Fake53{};
struct Fake54{};
struct Fake55{};


    }
}

#endif
