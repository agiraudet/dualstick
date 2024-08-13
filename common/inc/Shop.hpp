#ifndef SHOP_HPP
#define SHOP_HPP

#include "InteractiveTile.hpp"
#include "Player.hpp"
#include <memory>

#define SHOP_DEFAULT_PRICE 10

enum ShopType { NONE_SHOP = -1, AMMO_SHOP = 0 };

class IShop : public InteractiveTile {
public:
  static std::shared_ptr<IShop> createShop(ShopType type, int x, int y,
                                           int price);

public:
  IShop();
  IShop(ShopType type, int price);
  IShop(ShopType type, int x, int y, int price);
  IShop(const IShop &other);
  IShop &operator=(const IShop &other);
  ~IShop();

  int getPrice() const;
  void setPrice(int price);
  ShopType getType() const;
  void setType(ShopType type);

  void interact(Player &player) override;
  virtual void buy(Player &player) = 0;

protected:
  int _price;
  ShopType _type;
};

class AmmoShop : public IShop {
public:
  AmmoShop();
  AmmoShop(int price);
  AmmoShop(int x, int y, int price);
  AmmoShop(const AmmoShop &other);
  AmmoShop &operator=(const AmmoShop &other);
  ~AmmoShop();
  void buy(Player &player) override;
};

#endif // SHOP_HPP
