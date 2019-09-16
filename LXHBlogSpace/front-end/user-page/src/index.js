import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import * as serviceWorker from './serviceWorker';

import Navbar from './components/Navbar';
import MainApp from "./components/MainApp"

ReactDOM.render(<Navbar />, document.getElementById('app-bar'));
ReactDOM.render(<MainApp />, document.getElementById('left-panel'));
// If you want your app to work offline and load faster, you can change
// unregister() to register() below. Note this comes with some pitfalls.
// Learn more about service workers: https://bit.ly/CRA-PWA
serviceWorker.unregister();
