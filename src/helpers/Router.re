type previousNextAnimal = option((TokenId.t, TokenId.t));
type animalPageState =
  | DetailView(option(TokenId.t))
  | NormalView;
type leaderBoard =
  | TotalContribution
  | TotalDaysHeld
  | Unknown
  | MonthlyContribution;
type urlState =
  | User(Web3.ethAddress)
  | Org(string)
  | Explorer(animalPageState)
  | Leaderboards(leaderBoard)
  // | Unknown
  | Home(animalPageState)
  | IncreaseVoteIteration
  | VotePage;

let useUrlState = () => {
  let url = ReasonReactRouter.useUrl();

  React.useMemo1(
    () => {
      switch (Js.String.split("/", url.hash)) {
      | [|"user", address|] => User(address->Js.String.toLowerCase)
      | [|"org", orgId|] => Org(orgId->Js.String.toLowerCase)
      | [|"leaderboards", leaderboardType|] =>
        switch (leaderboardType) {
        | "monthly-contribution" => Leaderboards(MonthlyContribution)
        | "days-held" => Leaderboards(TotalDaysHeld)
        | "total-contribution" => Leaderboards(TotalContribution)
        | _ => Leaderboards(Unknown)
        }
      // | [|"explorer"|] => Explorer(NormalView)
      | [|"explorer", "details", animalStr|]
      | [|"explorer", "details", animalStr, ""|] =>
        let optionAnimal = Animal.getAnimal(animalStr);
        Explorer(DetailView(optionAnimal));
      // | [|"details"|] => Home(NormalView)
      | [|"details", animalStr|] =>
        let optionAnimal = Animal.getAnimal(animalStr);
        Home(DetailView(optionAnimal));
      | [|"dao"|] => VotePage
      | [|"increase-iteration"|] => IncreaseVoteIteration
      | urlArray =>
        switch (
          Belt.Array.get(urlArray, 0)->Belt.Option.mapWithDefault("", a => a)
        ) {
        | "explorer" => Explorer(NormalView)
        | _ => Home(NormalView)
        // | _ => Unknown
        }
      }
    },
    [|url.hash|],
  );
};
let useIsExplorer = () => {
  let urlState = useUrlState();

  React.useMemo1(
    () =>
      switch (urlState) {
      | Explorer(_) => true
      | User(_)
      | Leaderboards(_)
      | Home(_)
      | Org(_)
      | IncreaseVoteIteration
      | VotePage => false
      },
    [|urlState|],
  );
};
let isDetailsAnimalPage: animalPageState => bool =
  animalPageState =>
    switch (animalPageState) {
    | DetailView(_) => true
    | NormalView => false
    };
let useIsDetails = () => {
  let urlState = useUrlState();

  React.useMemo1(
    () =>
      switch (urlState) {
      | Explorer(inside) => isDetailsAnimalPage(inside)
      | Home(inside) => isDetailsAnimalPage(inside)
      | User(_)
      | Org(_)
      | Leaderboards(_)
      | IncreaseVoteIteration
      | VotePage => false
      },
    [|urlState|],
  );
};
let useIsHome = () => {
  let urlState = useUrlState();

  React.useMemo1(
    () =>
      switch (urlState) {
      | Home(_) => true
      | User(_)
      | Org(_)
      | Explorer(_)
      | Leaderboards(_)
      | IncreaseVoteIteration
      | VotePage => false
      },
    [|urlState|],
  );
};
let getAnimalFormAnimalPageState: animalPageState => option(TokenId.t) =
  animalPageState =>
    switch (animalPageState) {
    | DetailView(optAnimal) => optAnimal
    | NormalView => None
    };

let useAnimalForDetails = () => {
  let urlState = useUrlState();

  React.useMemo1(
    () =>
      switch (urlState) {
      | Explorer(animalPageState) =>
        getAnimalFormAnimalPageState(animalPageState)
      | Home(animalPageState) =>
        getAnimalFormAnimalPageState(animalPageState)
      // | DetailView(_, optAnimal) => optAnimal
      | User(_)
      | Org(_)
      | Leaderboards(_)
      | IncreaseVoteIteration
      | VotePage => None
      },
    [|urlState|],
  );
};
