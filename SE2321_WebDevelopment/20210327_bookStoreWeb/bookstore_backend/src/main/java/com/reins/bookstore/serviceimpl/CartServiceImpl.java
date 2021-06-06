package com.reins.bookstore.serviceimpl;

import com.reins.bookstore.dao.CartDao;
import com.reins.bookstore.entity.Cart;
import com.reins.bookstore.service.CartService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class CartServiceImpl implements CartService {
    @Autowired
    private CartDao cartDao;

    @Override
    public List<Cart> getCart() {
        return cartDao.getCart();
    }

    @Override
    public List<Cart> clearCart() {
        return cartDao.clearCart();
    }
}
