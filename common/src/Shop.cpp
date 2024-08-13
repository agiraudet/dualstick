#include "Shop.hpp"
#include "InteractiveTile.hpp"

std::shared_ptr<IShop> IShop::createShop(ShopType type, int x, int y,
                                         int price) {
  switch (type) {
  case AMMO_SHOP:
    return std::make_shared<AmmoShop>(x, y, price);
  default:
    return nullptr;
  }
}

IShop::IShop()
    : InteractiveTile(), _price(SHOP_DEFAULT_PRICE), _type(NONE_SHOP) {}

IShop::IShop(ShopType type, int price)
    : InteractiveTile(), _price(price), _type(type) {}

IShop::IShop(ShopType type, int x, int y, int price)
    : InteractiveTile(x, y), _price(price), _type(type) {}

IShop::IShop(const IShop &other)
    : InteractiveTile(other), _price(other._price), _type(other._type) {}

IShop &IShop::operator=(const IShop &other) {
  if (this != &other) {
    InteractiveTile::operator=(other);
    _price = other._price;
    _type = other._type;
  }
  return *this;
}

IShop::~IShop() {}

int IShop::getPrice() const { return _price; }

void IShop::setPrice(int price) { _price = price; }

ShopType IShop::getType(void) const { return _type; }

void IShop::setType(ShopType type) { _type = type; }

void IShop::interact(Player &player) { buy(player); }

// AMMOSHOP

AmmoShop::AmmoShop() : IShop() {}

AmmoShop::AmmoShop(int price) : IShop(AMMO_SHOP, price) {}

AmmoShop::AmmoShop(int x, int y, int price) : IShop(AMMO_SHOP, x, y, price) {}

AmmoShop::AmmoShop(const AmmoShop &other) : IShop(other) {}

AmmoShop &AmmoShop::operator=(const AmmoShop &other) {
  if (this != &other) {
    IShop::operator=(other);
  }
  return *this;
}

AmmoShop::~AmmoShop() {}

void AmmoShop::buy(Player &player) {
  if (player.removeMoney(_price)) {
    player.weapon->ammo = player.weapon->maxAmmo;
  }
}
