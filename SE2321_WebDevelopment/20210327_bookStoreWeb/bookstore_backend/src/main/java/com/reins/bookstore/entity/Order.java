package com.reins.bookstore.entity;


import com.fasterxml.jackson.annotation.JsonIdentityInfo;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.ObjectIdGenerators;
import lombok.Data;

import javax.persistence.*;
import java.util.List;
import java.util.Set;

@Data
@Entity
@Table(name = "orders")
@JsonIgnoreProperties(value = {"handler","hibernateLazyInitializer","fieldHandler"})
@JsonIdentityInfo(generator = ObjectIdGenerators.PropertyGenerator.class,property = "orderId")
public class Order {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "order_id")
    private Integer orderId;
    private Integer user_id;
    private Integer order_price;


    public Order(Integer user_id, Integer order_price, String date, String year, String month, String day) {
        this.user_id = user_id;
        this.order_price = order_price;
        this.date = date;
        this.year = year;
        this.month = month;
        this.day = day;
    }

    private String date;
    private String year;
    private String month;
    private String day;


    public Order() {

    }
}
