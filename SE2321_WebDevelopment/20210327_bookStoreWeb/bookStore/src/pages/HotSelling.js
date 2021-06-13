import React from 'react';

import Grid from '@material-ui/core/Grid';
import TextField from '@material-ui/core/TextField';
import Button from '../components/Button';
import BasicTable from "../components/hotBoard";
import axios from "axios"

export default class HotSelling extends React.Component {

  constructor(props) {
    super(props);
    this.state = {
      books:[],
      from:  "2021-04-05",
      to:"2021-05-04"
    }
  
      axios({
        method: 'GET',
        url: 'http://localhost:9090/getHotSelling',
        params: {
          from:this.state.from,
          to:this.state.to
        }
      }).then(response => {
       console.log(response);
        this.state.books=response.data;
      })
    }
    


  render() {
    // this.logRow();
    return (
      <div>
      
          <h1> Hot Selling Board</h1>
       
          <Grid container justify="space-around"  style={{ width: '100vh' }}>
      
        <TextField
          id="date"
          label="From"
          type="date"
          defaultValue="2021-05-10"
          InputLabelProps={{
            shrink: true,
          }}
          onChange={this.handleFromChange}
        />

        <TextField
          id="date"
          label="To"
          type="date"
          ref="myField"
          defaultValue="2021-06-11"
          onChange={this.handleToChange}
          InputLabelProps={{
            shrink: true,
          }}

        />
        <Button onClick={this.dateSelectBook}>select</Button>
        </Grid>

          <BasicTable
         
            rows={ this.state.books}
          />
  
      </div>
    );
  }
  
}

