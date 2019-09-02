import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import App from './App';
import MainContainer from "./components/MainContainer"
import * as serviceWorker from './serviceWorker';

ReactDOM.render(<App />, document.getElementById('nav-root'));
ReactDOM.render(<MainContainer />, document.getElementById('main-panel-root'));
ReactDOM.render(<div>footer</div>, document.getElementById('footer-root'));
// If you want your app to work offline and load faster, you can change
// unregister() to register() below. Note this comes with some pitfalls.
// Learn more about service workers: https://bit.ly/CRA-PWA
serviceWorker.unregister();
