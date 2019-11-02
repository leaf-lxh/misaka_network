import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import App from './App';
import MainContainer from "./components/MainContainer"
import * as serviceWorker from './serviceWorker';

//import Snackbar from "@material-ui/core/Snackbar";

ReactDOM.render(<App />, document.getElementById('nav-root'), ()=>{ReactDOM.render(MainContainer("main"), document.getElementById('main-panel-root'));});
//ReactDOM.render(<Snackbar open={true} message={"notice"} />, document.getElementById('notice-snackbar'));
// If you want your app to work offline and load faster, you can change
// unregister() to register() below. Note this comes with some pitfalls.
// Learn more about service workers: https://bit.ly/CRA-PWA
serviceWorker.unregister();
