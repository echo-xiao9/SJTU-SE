package com.reins.bookstore.service;
import com.reins.bookstore.entity.Order;
import com.reins.bookstore.entity.OrderItem;

import java.util.ArrayList;
import java.util.List;

public interface OrderService {
    Order findOrderById(Integer id);

    List<Order> getOrders();

    Order addOrderFromUser(Integer user_id, Integer order_price);

    ArrayList getAdminOrder();

//    List<OrderItem> getOrderItems();
}
