import React, { Component } from "react";
import ReactDOM from "react-dom";
import BasicTable from "../components/OrderCard"
import axios from "axios"
import { Link } from 'react-router-dom';
import { makeStyles } from '@material-ui/core/styles';
import Table from '@material-ui/core/Table';
import TableBody from '@material-ui/core/TableBody';
import TableCell from '@material-ui/core/TableCell';
import TableContainer from '@material-ui/core/TableContainer';
import TableHead from '@material-ui/core/TableHead';
import TableRow from '@material-ui/core/TableRow';
import Paper from '@material-ui/core/Paper';



export default class Orders extends React.Component {

  constructor(props) {
    super(props);
    this.state = {
      orders: [],
      newOrders:[]
    }
    const url = "http://localhost:9090/getAdminAllOrder";
    axios.get(url).then((response) => {
      const data = response.data;
      this.setState({
        orders: data
      })
     
      // console.log(this.state.orders);
      this.processData();
    })
  }


  processData(){
    this.state.orders.map((line)=>{
      var lineRow =new Array();
      // console.log(line[6]);
      // var rows=this.createRow(line[6].book_id,line[6].book_name,line[6].book_price,line[6].book_num);
      // var rows2=this.createData(line[0],line[1],line[2]);
      // console.log(rows);
      console.log(line[5]);
      const url = "http://localhost:9090/getAdminAllOrder";
      axios({
        method: 'GET',
        url: 'http://localhost:9090/getOrderItems',
        params: {
          order_id:line[5]
        }
      }).then(response=>{
        var row=response.data;
        var singleOrder=this.createOrderData(line[0],line[1],line[2],line[3],line[4],line[5],row);
        console.log("single Order:")
        console.log(singleOrder);
        this.state.newOrders.push(singleOrder);
        this.setState({
          orders:this.state.orders,
          newOrders:this.state.newOrders
        });
        console.log(this.state.newOrders);
      })
    })
    console.log(this.state.newOrders);

  }


  // createData(book_name, book_price, book_num ) {
  //   return { book_name, book_price, book_num };
  // }
 

  createOrderData(orderPrice,user_id,year,month,day, order_id,row){
    return {orderPrice,user_id,year,month,day, order_id,row};
  }
  // createRow(book_id,book_name,book_price,book_num){
  //   return {book_id,book_name,book_price,book_num};
  // }
  // rows = [
  //   this.createData('Frozen yoghurt', 159, 6.0),
  //   this.createData('Ice cream sandwich', 237),
  //   this.createData('Eclair', 262, 16.0),
  //   this.createData('Cupcake', 305, 3.7),
  //   this.createData('Gingerbread', 356, 16.0,)
  // ]

  // rows2 = [
  //   this.createData('Frozen yoghurt2', 159, 6.0),
  //   this.createData('Ice cream sandwich2', 237),
  //   this.createData('Eclair2', 262, 16.0),
  //   this.createData('Cupcake2', 305, 3.7),
  //   this.createData('Gingerbread2', 356, 16.0)
  // ]
  // rowAll = [this.rows, this.rows2]

  // obj = {
  //   row: this.rows,
  //   orderPrice: 1000,
  //   user_id: 1,
  //   year: 2021,
  //   month: 9,
  //   day: 2,
  //   order_id: 1
  // }
  // obj2 = {
  //   row: this.rows,
  //   orderPrice: 1000,
  //   user_id: 1,
  //   year: 2020,
  //   month: 9,
  //   day: 2,
  //   order_id: 1
  // }
  // objList=[this.obj,this.obj2]


  logRow() {
    console.log(this.objList);
  }

  render() {
    // this.logRow();
    return (
      <div>
        <h1> Order</h1>
        {this.state.newOrders.map((obj) => (
          <BasicTable
            orderPrice={obj.orderPrice}
            user_id={obj.user_id}
            year={obj.year}
            month={obj.month}
            day={obj.day}
            order_id={obj.order_id}
            rows={obj.row}
          />
        )
        )}

      </div>
    );
  }
}
