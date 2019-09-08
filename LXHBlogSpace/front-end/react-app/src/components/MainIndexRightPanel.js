import React from 'react';

import UserPanel from './UserPanel';
import HotTopic from './HotTopic';

function MainIndexRightPanel (){
    return(
        <>
            <div className="user-panel">
                <UserPanel  />
            </div>
            
            <div className="recommand-panel">
                <HotTopic />
            </div>
            
        </>
    )
}

export default MainIndexRightPanel;