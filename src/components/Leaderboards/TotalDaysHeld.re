open ReasonApolloHooks;

module LoadMostDaysHeld = [%graphql
  {|
    query {
      patronNews(first: 20, orderBy: totalTimeHeld, orderDirection: desc,  where: {id_not: "NO_OWNER"}) {
        id
        totalTimeHeld @bsDecoder(fn: "QlHooks.decodeBN")
        tokens{
          id
        }
        lastUpdated @bsDecoder(fn: "QlHooks.decodeBN")
      }
    }
  |}
];

let useLoadMostDaysHeld = () =>
  ApolloHooks.useSubscription(LoadMostDaysHeld.definition);
let useLoadMostDaysHeldData = () => {
  let (simple, _) = useLoadMostDaysHeld();
  let currentTimestamp = QlHooks.useCurrentTime();
  switch (simple) {
  | Data(largestContributors) =>
    let dailyContributions =
      largestContributors##patronNews
      |> Js.Array.map(patron => {
           let numberOfTokens = patron##tokens->Js.Array.length->string_of_int;
           let timeElapsed =
             BN.new_(currentTimestamp)->BN.subGet(. patron##lastUpdated);

           let totalTimeHeldWei =
             patron##totalTimeHeld
             ->BN.addGet(.
                 timeElapsed->BN.mulGet(. BN.new_(numberOfTokens)),
               );

           (patron##id, totalTimeHeldWei);
         });
    Array.sort(
      ((_, first), (_, second)) => {second->BN.cmpGet(. first)},
      dailyContributions,
    );
    Some(dailyContributions);
  | Error(_)
  | Loading
  | NoData => None
  };
};

open Css;

let flameImg = "/img/streak-flame.png";
let goldTrophyImg = "/img/icons/gold-trophy.png";
let silverTrophyImg = "/img/icons/silver-trophy.png";
let bronzeTrophyImg = "/img/icons/bronze-trophy.png";

let leaderboardTable =
  style([
    width(`percent(100.)),
    tableLayout(`fixed),
    overflowWrap(`breakWord),
  ]);

let leaderboardHeader = style([backgroundColor(`hex("73c7d7ff"))]);

let streakTextLeaderboard =
  style([
    position(absolute),
    zIndex(100),
    bottom(`percent(-10.)),
    right(`percent(50.)),
    transform(translateX(`px(-5))),
  ]);
let flameImgLeaderboard =
  style([width(`percent(100.)), maxWidth(px(50))]);

let rankText =
  style([
    position(absolute),
    zIndex(100),
    bottom(`percent(-10.)),
    right(`percent(50.)),
    transform(translate(`px(-4), `px(-15))),
  ]);

let trophyImg =
  style([width(`percent(100.)), width(px(50)), height(px(50))]);

let centerFlame =
  style([
    display(block),
    margin(auto),
    width(`px(70)),
    position(relative),
  ]);

let rankMetric = style([fontSize(`px(16))]);

let rankingColor = index =>
  style([
    backgroundColor(`hex(index mod 2 == 1 ? "b5b5bd22" : "ffffffff")),
  ]);

module ContributorsRow = {
  [@react.component]
  let make = (~contributor, ~amount, ~index) => {
    UserProvider.useUserInfoContext().update(contributor, false);

    let optThreeBoxData = UserProvider.use3BoxUserData(contributor);
    let optUserName =
      Belt.Option.(
        optThreeBoxData
        ->flatMap(threeBoxData => threeBoxData.profile)
        ->flatMap(threeBoxData => threeBoxData.name)
      );
    let clearAndPush = RootProvider.useClearNonUrlStateAndPushRoute();

    <tr key=contributor className={rankingColor(index)}>
      <td>
        <span className=centerFlame>
          {index == 0
             ? <img className=trophyImg src=goldTrophyImg />
             : index == 1
                 ? <img className=trophyImg src=silverTrophyImg />
                 : index == 2
                     ? <img className=trophyImg src=bronzeTrophyImg />
                     : <div className=trophyImg />}
          <p className=rankText>
            <strong>
              "#"->React.string
              {(index + 1)->string_of_int->React.string}
            </strong>
          </p>
        </span>
      </td>
      <td>
        <a
          onClick={e => {
            ReactEvent.Mouse.preventDefault(e);
            clearAndPush({j|/#user/$contributor|j});
          }}>
          {switch (optUserName) {
           | Some(name) => <span> name->React.string </span>
           | None =>
             <span>
               {{
                  Helper.elipsify(contributor, 20);
                }
                ->React.string}
             </span>
           }}
        </a>
      </td>
      //  <td>
      //    <span className=centerFlame>
      //      <img className=flameImgLeaderboard src=flameImg />
      //      <p className=streakTextLeaderboard>
      //        <strong> "1"->React.string </strong>
      //      </p>
      //    </span>
      //  </td>
      <td className=rankMetric> {(amount ++ " Days")->React.string} </td>
    </tr>;
  };
};

module MostDaysHeld = {
  [@react.component]
  let make = (~mostDaysHeld) => {
    React.array(
      Array.mapi(
        (index, (contributor, amount)) => {
          <ContributorsRow
            contributor
            amount={
              // ->BN.new_("86400")
              // There are 86400 seconds in a day.
              amount->BN.divGet(. BN.new_("86400"))->BN.toStringGet(.)
            }
            index
          />
        },
        mostDaysHeld,
      ),
    );
  };
};

[@react.component]
let make = (~numberOfLeaders) => {
  let mostDaysHeldOpt = useLoadMostDaysHeldData();

  <div>
    <Rimble.Heading>
      "Wildcards Accumulative Days Held Leaderboard"->React.string
    </Rimble.Heading>
    <br />
    <Rimble.Table className=leaderboardTable>
      <thead className=leaderboardHeader>
        <tr>
          <th> "Rank"->React.string </th>
          <th> "Guardian"->React.string </th>
          // <th> "Longest Streak"->React.string </th>
          <th> "Accumulative Days Held"->React.string </th>
        </tr>
      </thead>
      <tbody>
        {switch (mostDaysHeldOpt) {
         | Some(mostDaysHeldFull) =>
           let mostDaysHeld =
             Belt.Array.slice(
               mostDaysHeldFull,
               ~offset=0,
               ~len=numberOfLeaders,
             );
           <MostDaysHeld mostDaysHeld />;
         | None => React.null
         }}
      </tbody>
    </Rimble.Table>
  </div>;
};
